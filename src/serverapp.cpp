#include "message.hpp"

#include <boost/asio.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <set>

using boost::asio::ip::tcp;
namespace ba = boost::asio;

class user {
public:
    virtual void do_send(message &msg) = 0;
    virtual ~user(){};
};

typedef std::shared_ptr<user> user_ptr;

//--------------------------------------------

class chatroom {
private:
    std::set<user_ptr> users_;

public:
    void add_user(user_ptr user) { users_.insert(user); }

    void send_to_everyone(message &msg) {
        for (const auto user : users_) {
            user->do_send(msg);
        }
    }

    void leave(user_ptr user) { users_.erase(user); }
};

//--------------------------------------------

class connection : public user,
                   public std::enable_shared_from_this<connection> {
private:
    /*we need this socket to live for as long as there any reference to it so we
     * use std::enable_shared_from_this to achieve it*/
    tcp::socket socket_;
    /*participant need to know in which chat room he is*/
    chatroom &chatroom_;

    message msg;

public:
    connection(tcp::socket socket, chatroom &cr)
        : socket_(std::move(socket)), chatroom_(cr) {}

    void start() {
        chatroom_.add_user(shared_from_this());
        /* do_read() -> do_send() -> do_read() -> ... */
        do_read();
    }

    void leave() { chatroom_.leave(shared_from_this()); }

    ~connection() { socket_.close(); }

    virtual void do_send(message &msg) override {
        ba::async_write(socket_, ba::buffer(msg.data(), message::full_length),
                        [&](boost::system::error_code err, size_t /*len*/) {
                            if (!err) {
                                do_read();
                            } else {
                                std::cerr << "send() error" << '\n';
                            }
                        });
    };

private:
    void do_read() {
        ba::async_read(socket_, ba::buffer(msg.data(), message::full_length),
                       [&](boost::system::error_code err, size_t /*len*/) {
                           if (!err && msg.is_valid_message()) {
                               chatroom_.send_to_everyone(msg);
                           } else {
                               std::cerr << "read() error" << '\n';
                           }
                       });
    }
};

//--------------------------------------------

class chat {
private:
    ba::io_context &io_;
    tcp::acceptor acceptor_;
    chatroom chatroom_;

public:
    chat(ba::io_context &io)
        : io_(io), acceptor_(io, tcp::endpoint(tcp::v4(), 8783)) {
        connect_users();
    }

    void connect_users() {
        static tcp::endpoint endp;
        acceptor_.async_accept(endp, [&](const boost::system::error_code &err,
                                         tcp::socket socket) {
            std::clog << "Connected user " << endp << '\n';
            if (!err) {
                /* here is code that enables message
                 * communications between participants */
                std::make_shared<connection>(std::move(socket), chatroom_)
                    ->start();
            }
            /* after aynchronously handling one connect,
             * recursivly call asynchronous connect to
             * handle another connect */
            connect_users();
        });
    };
};

//--------------------------------------------

int main() {

    try {
        boost::asio::io_context io;
        chat chat(io);
        io.run();
    } catch (const std::exception &e) { std::cerr << e.what() << '\n'; }

    return 0;
}