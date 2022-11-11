#pragma once

#include "message.hpp"

#include <boost/asio.hpp>
#include <deque>
#include <iostream>
#include <memory>
#include <set>

namespace chat {

using boost::asio::ip::tcp;
namespace ba = boost::asio;

/**
 * Abstraction over user.
 *
 * It is used for class chatroom to store ptr to this virtual class instead of
 * to ptr to connection. Class connection stores reference to chatroom and it
 * could create a cyclic dependency, but we avoid it using inheritance.
 *
 */
class user {
public:
    virtual void do_send(message &msg) = 0;
    virtual ~user(){};
};

typedef std::shared_ptr<user> user_ptr;

//--------------------------------------------

/**
 * Wrapper around set of connections.
 *
 * It keeps track of all users and also stores some last sent messages.
 *
 */
class chatroom {
private:
    std::set<user_ptr> users_;

    static constexpr int recent_max_ = 100;
    std::deque<message> recent_messages_;

public:
    void add_user(user_ptr user) { users_.insert(user); }

    void leave(user_ptr user) { users_.erase(user); }

    void send_history(user_ptr user);

    void add_message(message &msg);

    void send_to_everyone(message &msg);
};

//--------------------------------------------

/**
 * Wrapper around tcp::socket.
 */
class connection : public user,
                   public std::enable_shared_from_this<connection> {
private:
    /*we need this class (and this socket) to live for as long as there any
     * reference to it so we use std::enable_shared_from_this to achieve it*/
    tcp::socket socket_;
    /*a user need to know in which chat room he is*/
    chatroom &chatroom_;
    /*message is stored as member variable so it is not deallocated during
     * async operations*/
    message msg_;

public:
    connection(tcp::socket socket, chatroom &cr)
        : socket_(std::move(socket)), chatroom_(cr) {}

    void start() {
        chatroom_.add_user(shared_from_this());
        chatroom_.send_history(shared_from_this());
        /* recursive chain */
        /* do_read() -> do_send() -> do_read() -> ... */
        do_read();
    }

    void leave() { chatroom_.leave(shared_from_this()); }

    ~connection() { socket_.close(); }

    /*send *to* the user over a socket*/
    virtual void do_send(message &msg) override;

    /*read *from* the user over a socket*/
    void do_read();
};

//--------------------------------------------

/**
 * Wrapper around tcp::acceptor and io_context.
 *
 * It accepts connections and stores them in chatroom.
 *
 */
class chat_server {
private:
    ba::io_context &io_;
    tcp::acceptor acceptor_;
    chatroom chatroom_;

public:
    chat_server(ba::io_context &io, int port)
        : io_(io), acceptor_(io, tcp::endpoint(tcp::v4(), port)) {
        /* recursive chain */
        /* connect_users() -> connect_users() -> ... */
        connect_users();
    }

private:
    void connect_users();
};

} // namespace chat