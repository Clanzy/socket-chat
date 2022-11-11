#pragma once

#include "message.hpp"
#include "ui.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;
namespace ba = boost::asio;

namespace chat {

class chat_client {
private:
    UI ui;
    ba::io_context &io_;
    tcp::socket socket_;
    /*these messages are stored locally so they are not deallocated during async
     * operations*/
    message read_message;
    message write_message;

    std::thread t1;

public:
    chat_client(ba::io_context &io, tcp::resolver::results_type &endpoints)
        : io_(io), socket_(io) {
        do_connect(endpoints);
        t1 = std::thread([&]() { io_.run(); });
        while (true) {
            char *m = ui.get_input();
            // TODO implement custom char* ctor instead of std::string default
            // ctor (it makes two deep copies instead of one)
            message msg(m);
            do_send(msg);
        }
    }

    void do_send(message &message) {
        write_message = message;
        do_write();
    }

    void close() {
        ba::post([&]() { socket_.close(); });
        t1.join();
    }

private:
    void do_connect(tcp::resolver::results_type &endpoints);

    void do_read();

    void do_write();

    
};

} // namespace chat