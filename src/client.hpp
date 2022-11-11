#pragma once

#include "message.hpp"
#include "ui.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <string>

namespace chat {

using boost::asio::ip::tcp;
namespace ba = boost::asio;

class client_socket {
protected:
    ba::io_context &io_;
    tcp::socket socket_;

    message read_message_;
    message write_message_;

    client_socket(ba::io_context &io, tcp::resolver::results_type &endpoints,
                  std::function<void(const char *)> fctn_print)
        : io_(io), socket_(io) {
        do_connect(endpoints, fctn_print);
    }

    /*connect to the server and specify which function will print messages*/
    void do_connect(tcp::resolver::results_type &endpoints,
                    std::function<void(const char *)> fctn_print);
    /*read *from* the server over a socket AND print it using fctn_print*/
    void do_read(std::function<void(const char *)> fctn_print);
    /*write *to* the server over a socket*/
    void do_write(message &message);

    virtual ~client_socket() {
        ba::post([&]() { socket_.close(); });
    }
};

class chat_client : private client_socket {
private:
    UI ui;

    std::thread io_thread;

public:
    /* allocate one thread for reading and writing messages from server. At this
     * time, the main thread is used for reading user input and sending it to
     * the server
     */
    chat_client(ba::io_context &io, tcp::resolver::results_type &endpoints);

    virtual ~chat_client() override { io_thread.join(); }
};

} // namespace chat