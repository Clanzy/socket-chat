#pragma once

#include "message.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <string>

using boost::asio::ip::tcp;
namespace ba = boost::asio;

namespace chat {

class chat_client {
private:
    ba::io_context &io_;
    tcp::socket socket_;
    /*these messages are stored locally so they are not deallocated during async
     * operations*/
    message read_message;
    message write_message;

public:
    chat_client(ba::io_context &io, tcp::resolver::results_type &endpoints)
        : io_(io), socket_(io) {
        do_connect(endpoints);
    }

    void do_send(message &message) {
        write_message = message;
        do_write();
    }

    void close() {
        ba::post([&]() { socket_.close(); });
    }

private:
    void do_connect(tcp::resolver::results_type &endpoints);

    void do_read();

    void do_write();
};

} // namespace chat