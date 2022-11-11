#pragma once

#include <boost/asio.hpp>
#include <cstring>
#include <stdexcept>

namespace chat {

constexpr int msg_size = 1020;

class BorderedWindow {
protected:
    void *window_;
    void *border_;

public:
    BorderedWindow(int height, int width, int starty, int startx);

    virtual ~BorderedWindow();
};

class OutputWindow : public BorderedWindow {
public:
    OutputWindow(int height, int width, int starty, int startx);

    void write(char *msg);
};

class InputWindow : public BorderedWindow {
private:
    char msg_[msg_size];

public:
    InputWindow(int height, int width, int starty, int startx);

    void move_to_start();

    char *read();
};

class Curses {
public:
    Curses();
    ~Curses();
};

class UI {
private:
    static constexpr int divisor = 5;
    Curses curses_;
    OutputWindow output_;
    InputWindow input_;

public:
    UI();
    char *get_input();
    void do_write(char *msg);
};

} // namespace chat
