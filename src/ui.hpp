#pragma once

#include "request.hpp"

#include <span>
namespace chat {

constexpr int request_length = request().length();
static constexpr int divisor = 6;

class BorderedWindow {
protected:
    void *window_;
    void *border_;

public:
    BorderedWindow(int height, int width, int starty, int startx);

    virtual ~BorderedWindow();
};

class OutputWindow : public BorderedWindow {
private:
    int height_;
    int width_;
    static constexpr int max_messages_ = 100;
    int curr_position_                 = -height_;

public:
    OutputWindow(int height, int width, int starty, int startx);

    void write(std::span<const char> username, std::span<const char> message, int color);

    void do_scroll();
};

class InputWindow : public BorderedWindow {
private:
    char msg_[request_length];

public:
    InputWindow(int height, int width, int starty, int startx);

    void move_to_start();

    void read(std::span<char> str);
};

class UsernameDialog : public BorderedWindow {
public:
    UsernameDialog();

    void get_input(std::span<char> str);
};

class Curses {
public:
    Curses();
    ~Curses();
};

class UI {
private:
    Curses curses_;
    OutputWindow output_;
    InputWindow input_;

    std::string username_;

public:
    UI();

    request ask_username();
    request ask_message();
    void display_message(const request &msg);
};

} // namespace chat
