#pragma once

#include "request.hpp"
#include "ui.hpp"

#include <boost/asio.hpp>
#include <deque>
#include <iostream>
#include <string>

namespace chat {

using boost::asio::ip::tcp;
namespace ba = boost::asio;

class chat_client {
private:
    ba::io_context &io_;
    tcp::socket socket_;

    request read_message_;
    std::deque<request> write_messages_;

    UI ui;

    std::thread io_thread;

public:
    chat_client(ba::io_context &io, tcp::resolver::results_type &endpoints);

    ~chat_client() {
        ba::post([&]() { socket_.close(); });
    }

    void deliver(const request &req);

private:
    /*connect to the server*/
    void do_connect(tcp::resolver::results_type &endpoints);
    /*read *from* the server over a socket AND print it using fctn_print*/
    void do_read();
    /*write *to* the server over a socket*/
    void do_write();
};

} // namespace chat