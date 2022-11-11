#include "server.hpp"

#include "message.hpp"

void chat::chatroom::send_history(user_ptr user) {
    for (auto &msg : recent_messages_) {
        user->do_send(msg);
    }
}

void chat::chatroom::send_to_everyone(message &msg) {
    add_message(msg);
    for (const auto user : users_) {
        user->do_send(msg);
    }
}

void chat::chatroom::add_message(message &msg) {
    recent_messages_.push_back(msg);
    if (recent_messages_.size() > recent_max_) {
        recent_messages_.pop_front();
    }
}

void chat::connection::do_send(message &msg) {
    ba::async_write(socket_, ba::buffer(msg.data(), message::capacity),
                    [&](boost::system::error_code err, size_t /*len*/) {
                        if (!err) {
                            do_read();
                        } else {
                            /*error handling*/
                        }
                    });
};

void chat::connection::do_read() {
    ba::async_read(socket_, ba::buffer(msg_.data(), message::capacity),
                   [&](boost::system::error_code err, size_t /*len*/) {
                       if (!err && msg_.length() != 0) {
                           chatroom_.send_to_everyone(msg_);
                       } else {
                           /*error handling*/
                       }
                   });
}

void chat::chat_server::connect_users() {
    /*static tcp::endpoint endp;*/
    acceptor_.async_accept(/*endp, */ [&](const boost::system::error_code &err,
                                          tcp::socket socket) {
        if (!err) {
            std::make_shared<connection>(std::move(socket), chatroom_)->start();
        }
        /* recursive chain */
        connect_users();
    });
};