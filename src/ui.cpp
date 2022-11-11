#include "ui.hpp"

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
    wborder((WINDOW *)border_, '0', '0', '0', '0', '0', '0', '0', '0');
    wclear((WINDOW *)window_);
    wrefresh((WINDOW *)border_);
    delwin((WINDOW *)border_);
    delwin((WINDOW *)window_);
}

chat::OutputWindow::OutputWindow(int height, int width, int starty, int startx)
    : BorderedWindow(height, width, starty, startx) {
    scrollok((WINDOW *)window_, TRUE);
}

void chat::OutputWindow::write(const char *msg) {
    wattron((WINDOW *)window_, COLOR_PAIR(1));
    wprintw((WINDOW *)window_, "%s", "Clanzy: ");
    wattroff((WINDOW *)window_, COLOR_PAIR(1));
    waddnstr((WINDOW *)window_, msg, msg_size);
    waddch((WINDOW *)window_, '\n');
    wrefresh((WINDOW *)window_);
}

chat::InputWindow::InputWindow(int height, int width, int starty, int startx)
    : BorderedWindow(height, width, starty, startx) {}

char *chat::InputWindow::read() {
    mvwgetnstr((WINDOW *)window_, 0, 0, msg_, msg_size);
    wclear((WINDOW *)window_);
    wrefresh((WINDOW *)window_);
    return msg_;
}

void chat::InputWindow::move_to_start() {
    wmove((WINDOW *)window_, 0, 0);
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

char *chat::UI::get_input() { return input_.read(); }
void chat::UI::do_write(const char *msg) {
    output_.write(msg);
    input_.move_to_start();
}