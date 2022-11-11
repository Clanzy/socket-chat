#include "client.hpp"

void chat::client_socket::do_connect(
    tcp::resolver::results_type &endpoints,
    std::function<void(const char *)> fctn_print) {
    ba::async_connect(socket_, endpoints,
                      [this, fctn_print](boost::system::error_code err,
                                         tcp::endpoint /*endp*/) {
                          if (!err) {
                              do_read(fctn_print);
                          } else {
                              throw std::runtime_error("Failed to connect");
                          }
                      });
}

void chat::client_socket::do_read(
    std::function<void(const char *)> fctn_print) {
    ba::async_read(
        socket_, ba::buffer(read_message_.data(), message::capacity),
        [this, fctn_print](boost::system::error_code err, size_t /*len*/) {
            if (!err && read_message_.length() != 0) {
                fctn_print(read_message_.data());
                do_read(fctn_print);
            } else {
                /*error handling*/
            }
        });
}

void chat::client_socket::do_write(message &message) {
    write_message_ = message;
    ba::async_write(socket_,
                    ba::buffer(write_message_.data(), message::capacity),
                    [this](boost::system::error_code err, size_t /*len*/) {
                        if (!err) {
                        } else {
                            /*error handling*/
                        }
                    });
}

chat::chat_client::chat_client(ba::io_context &io,
                               tcp::resolver::results_type &endpoints)
    : client_socket(io, endpoints,
                    [&ui = ui](const char *msg) { ui.do_write(msg); }) {

    io_thread = std::thread([&]() { io_.run(); });
    while (true) {
        char *m = ui.get_input();
        message msg(m);
        do_write(msg);
    }
}