#include "client.hpp"

#include "ui.hpp"

void chat::chat_client::do_connect(tcp::resolver::results_type &endpoints) {
    ba::async_connect(
        socket_, endpoints,
        [this](boost::system::error_code err, tcp::endpoint endp) {
            std::clog << "Connected to " << endp << '\n';
            if (!err) {
                /*set into async read mode, in paralel take input and async
                 * write*/
                do_read();
            }
        });
}

void chat::chat_client::do_read() {
    ba::async_read(socket_,
                   ba::buffer(read_message.data(), message::full_length),
                   [this](boost::system::error_code err, size_t /*len*/) {
                       if (!err && read_message.is_valid_message()) {
                           ui.do_write(read_message.msg());
                           do_read();
                       } else {
                           std::cerr << "Error while read" << '\n';
                       }
                   });
}

void chat::chat_client::do_write() {
    ba::async_write(socket_,
                    ba::buffer(write_message.data(), message::full_length),
                    [this](boost::system::error_code err, size_t /*len*/) {
                        if (!err) {
                        } else {
                            std::cerr << "Error while write" << '\n';
                        }
                    });
}