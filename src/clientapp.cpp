#include "client.hpp"

#include <string>
#include <thread>

int main(int argc, char *argv[]) {
    try {
        if (argc != 3) {
            std::cout << "Usage: chat <host> <port>" << '\n';
            return 1;
        }
        chat::ba::io_context io;
        chat::tcp::resolver res(io);
        auto endps = res.resolve(argv[1], argv[2]);
        const chat::chat_client c(io, endps);
        // block by ncurses
    } catch (std::exception &e) { std::cerr << e.what() << '\n'; }

    return 0;
}