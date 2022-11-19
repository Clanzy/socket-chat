#include "client.hpp"

chat::chat_client::chat_client(ba::io_context &io,
                               tcp::resolver::results_type &endpoints)
    : ui(), io_(io), socket_(io) {
    do_connect(endpoints);
    ui.ask_username();
    io_thread = std::thread([&]() { io_.run(); });
    while (true) {
        request req = ui.ask_message();
        deliver(req);
    }
}

void chat::chat_client::deliver(const request &req) {
    bool is_empty = write_messages_.empty();
    write_messages_.push_back(req);
    if (is_empty) {
        do_write();
    }
}

void chat::chat_client::do_connect(tcp::resolver::results_type &endpoints) {
    ba::async_connect(
        socket_, endpoints,
        [this](boost::system::error_code err, tcp::endpoint /*endp*/) {
            if (!err) {
                do_read();
            } else {
                throw std::runtime_error("Failed to connect");
            }
        });
}

void chat::chat_client::do_read() {
    ba::async_read(socket_, ba::buffer(read_message_.data(), read_message_.length()),
                   [this](boost::system::error_code err, size_t /*len*/) {
                       if (!err) {
                            // it's blocking
                            // should be done with queue and post()
                           ui.display_message(read_message_);
                           do_read();
                       } else {
                           /*error handling*/
                           socket_.close();
                       }
                   });
}

void chat::chat_client::do_write() {
    ba::async_write(socket_,
                    ba::buffer(write_messages_.front().data(),
                               write_messages_.front().length()),
                    [this](boost::system::error_code err, size_t /*len*/) {
                        if (!err) {
                            write_messages_.pop_front();
                            if(!write_messages_.empty()){
                                do_write();
                            }
                        } else {
                            /*error handling*/
                            socket_.close();
                        }
                    });
}
