#include "client.hpp"

#include <boost/asio.hpp>
#include <string>
#include <thread>

int main(int argc, char *argv[]) {
    try {
        if (argc != 3) {
            std::cout << "Usage: chat <host> <port>" << '\n';
            return 1;
        }
        ba::io_context io;
        tcp::resolver res(io);
        auto endps = res.resolve(argv[1], argv[2]);
        chat::chat_client c(io, endps);
        std::thread t([&]() { io.run(); });
        std::string line;
        while (std::getline(std::cin, line, '\n')) {
            chat::message m(line);
            c.do_send(m);
        }
        c.close();
        t.join();
    } catch (std::exception &e) { std::cerr << e.what() << '\n'; }

    return 0;
}