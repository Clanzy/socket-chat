#include "server.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <list>

int main(int argc, char *argv[]) {
    try {
        if (argc < 2) {
            std::cout << "Usage: chat_server <port> [<port> ...]" << '\n';
            return 1;
        }
        boost::asio::io_context io;
        std::list<chat::chat_server> l;
        for (int i = 1; i < argc; i++) {
            l.emplace_back(io, std::stoi(argv[i]));
        }
        io.run();
    } catch (const std::exception &e) { std::cerr << e.what() << '\n'; }

    return 0;
}