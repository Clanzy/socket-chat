#include "ui.hpp"

#include "request.hpp"

#include <cmath>
#include <ncurses.h>

chat::BorderedWindow::BorderedWindow(int height, int width, int starty,
                                     int startx)
    : border_(newwin(height, width, starty, startx)),
      window_(newwin(height - 2, width - 2, starty + 1, startx + 1)) {
    box((WINDOW *)border_, 0, 0);
    wrefresh((WINDOW *)border_);
    wrefresh((WINDOW *)window_);
}

chat::BorderedWindow::~BorderedWindow() {
    wborder((WINDOW *)border_, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wclear((WINDOW *)window_);
    wrefresh((WINDOW *)border_);
    delwin((WINDOW *)border_);
    delwin((WINDOW *)window_);
}

chat::OutputWindow::OutputWindow(int height, int width, int starty, int startx)
    : BorderedWindow(height, width, starty, startx), height_{height - 1 - 1},
      width_{width - 1 - 1} {
    delwin((WINDOW *)window_);
    window_ = newpad(max_messages_, width_);
    prefresh((WINDOW *)window_, 0, 0, 1, 1, height_, width_);
    scrollok((WINDOW *)window_, TRUE);
}

void chat::OutputWindow::write(std::span<const char> username,
                               std::span<const char> message, int color) {
    wattron((WINDOW *)window_, COLOR_PAIR(color));
    wprintw((WINDOW *)window_, "%s: ", username.data());
    wattroff((WINDOW *)window_, COLOR_PAIR(color));
    waddnstr((WINDOW *)window_, message.data(), request_length);
    waddch((WINDOW *)window_, '\n');
    int x = ceil((double)(username.size() + 2 + message.size() - 1) / width_);
    if (curr_position_ < max_messages_ - LINES + divisor + 1) {
        curr_position_ += x;
    }
    prefresh((WINDOW *)window_, curr_position_, 0, 1, 1, height_, width_);
}

void chat::OutputWindow::do_scroll() {
    wmove((WINDOW *)border_, 0, 0);
    noecho();
    keypad((WINDOW *)window_, TRUE);
    int index = curr_position_;
    int ch;
    while ((ch = wgetch((WINDOW *)window_)) != 'q') {
        switch (ch) {
        case KEY_UP:
            if (index > 0) {
                --index;
            }
            prefresh((WINDOW *)window_, index, 0, 1, 1, height_, width_);
            break;
        case KEY_DOWN:
            if (index < curr_position_) {
                ++index;
            }
            prefresh((WINDOW *)window_, index, 0, 1, 1, height_, width_);
            break;
        }
    }
    prefresh((WINDOW *)window_, curr_position_, 0, 1, 1, height_, width_);
    echo();
    keypad((WINDOW *)window_, FALSE);
}

chat::InputWindow::InputWindow(int height, int width, int starty, int startx)
    : BorderedWindow(height, width, starty, startx) {}

void chat::InputWindow::read(std::span<char> str) {
    mvwgetnstr((WINDOW *)window_, 0, 0, str.data(), str.size() - 1);
    wclear((WINDOW *)window_);
    wrefresh((WINDOW *)window_);
}

void chat::InputWindow::move_to_start() {
    wmove((WINDOW *)window_, 0, 0);
    wrefresh((WINDOW *)window_);
}

chat::UsernameDialog::UsernameDialog()
    : BorderedWindow(5, request().username_length() + 2 + 1, (LINES - 5) / 2,
                     (COLS - request().username_length() + 2) / 2) {
    mvwprintw((WINDOW *)window_, 0, 0, "Username:\n\n");
    wrefresh((WINDOW *)window_);
}

void chat::UsernameDialog::get_input(std::span<char> str) {
    wmove((WINDOW *)window_, 3, 0);
    wgetnstr((WINDOW *)window_, str.data(), str.size() - 1);
    clrtoeol();
    wrefresh((WINDOW *)window_);
}

chat::Curses::Curses() {
    initscr();
    refresh();

    if (!has_colors()) {
        throw std::runtime_error("Your terminal does not support colors");
    }
    start_color();

    init_pair(1, COLOR_BLUE, COLOR_BLACK);  /*your username*/
    init_pair(2, COLOR_GREEN, COLOR_BLACK); /*others username*/
}

chat::Curses::~Curses() { endwin(); }

chat::UI::UI()
    : output_(LINES - divisor, COLS, 0, 0),
      input_(divisor, COLS, LINES - divisor, 0) {}

chat::request chat::UI::ask_username() {
    static char buffer[request().username_length() + 1];
    UsernameDialog d;
    request req;
    while (true) {
        d.get_input(buffer);
        try {
            req.encode_action(Action::USERNAME);
            req.encode_username(buffer);
            req.encode_message("");
            username_ = buffer;
            break;
        } catch (std::exception &e) {}
    }
    return req;
}

chat::request chat::UI::ask_message() {
    static char buffer[request().message_length() + 1];
    request req;
    while (true) {
        input_.read(buffer);
        if (strcmp(buffer, "/scroll") == 0) {
            output_.do_scroll();
            continue;
        }
        try {
            req.encode_action(Action::MESSAGE);
            req.encode_username(username_);
            req.encode_message(buffer);
            break;
        } catch (std::exception &e) {}
    }
    return req;
}

void chat::UI::display_message(const request &msg) {
    Action act = msg.action();
    switch (act) {
    case Action::MESSAGE:
        if (strcmp(msg.username(), username_.data()) == 0)
            output_.write({msg.username(), strlen(msg.username() + 1)},
                          {msg.message(), strlen(msg.message()) + 1}, 1);
        else
            output_.write({msg.username(), strlen(msg.username() + 1)},
                          {msg.message(), strlen(msg.message()) + 1}, 2);
    default:
        /*should never be there*/
        //throw std::runtime_error("Wrong request");
        break;
    }

    input_.move_to_start();
}
