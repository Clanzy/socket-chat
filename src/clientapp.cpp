#include "message.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <thread>

using boost::asio::ip::tcp;
namespace ba = boost::asio;

class chat {
private:
    ba::io_context &io_;
    tcp::socket socket_;

    message read_message;
    message write_message;

public:
    chat(ba::io_context &io, tcp::resolver::results_type &endpoints)
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
    void do_connect(tcp::resolver::results_type &endpoints) {
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
    void do_read() {
        ba::async_read(
            socket_, ba::buffer(read_message.data(), message::full_length),
            [this](boost::system::error_code err, size_t /*len*/) {
                if (!err && read_message.is_valid_message()) {
                    std::cout.write(read_message.msg(), read_message.length());
                    std::cout << '\n';
                    do_read();
                } else {
                    std::cerr << "Error while read" << '\n';
                }
            });
    }
    void do_write() {
        ba::async_write(
            socket_, ba::buffer(write_message.data(), message::full_length),
            [this](boost::system::error_code err, size_t /*len*/) {
                if (!err) {
                } else {
                    std::cerr << "Error while write" << '\n';
                }
            });
    }
};

int main(/*int argc, char *argv[]*/) {

    try {
        ba::io_context io;
        tcp::resolver res(io);
        auto endps = res.resolve("127.0.0.1", "8783");
        chat c(io, endps);
        std::thread t([&]() { io.run(); });
        std::string line;
        while (std::getline(std::cin, line, '\n')) {
            message m(line);
            c.do_send(m);
        }
        c.close();
        t.join();
    } catch (std::exception &e) { std::cerr << e.what() << '\n'; }

    return 0;
}