#include "server.hpp"

#include "request.hpp"

void chat::chatroom::send_history(user_ptr user) {
    for (auto &msg : recent_messages_) {
        user->deliver(msg);
    }
}

void chat::chatroom::send_to_everyone(const request &msg) {
    add_request(msg);
    for (const auto user : users_) {
        user->deliver(msg);
    }
}

void chat::chatroom::add_request(const request &msg) {
    recent_messages_.push_back(msg);
    if (recent_messages_.size() > recent_max_) {
        recent_messages_.pop_front();
    }
}

void chat::connection::username(const request &req) {
    username_ = req.username();
}

void chat::connection::deliver(const request &msg) {
    bool is_empty = message_queue_.empty();
    message_queue_.push_back(msg);
    if (is_empty) {
        do_send();
    }
}

void chat::connection::do_send() {
    ba::async_write(socket_,
                    ba::buffer(message_queue_.front().data(),
                               message_queue_.front().length()),
                    [&](boost::system::error_code err, size_t /*len*/) {
                        if (!err) {
                            message_queue_.pop_front();
                            if (!message_queue_.empty()) {
                                do_send();
                            }
                        } else {
                            /*error handling*/
                            leave();
                        }
                    });
};

void chat::connection::do_read() {
    ba::async_read(socket_, ba::buffer(read_request_.data(), read_request_.length()),
                   [&](boost::system::error_code err, size_t /*len*/) {
                       if (!err) {
                           Action act = read_request_.action();
                           switch (act) {
                           case Action::USERNAME:
                               username(read_request_);
                               break;
                           case Action::MESSAGE:
                               chatroom_.send_to_everyone(read_request_);
                               break;
                           default:
                               /*should never be there*/
                               leave();
                               break;
                           }
                           do_read();
                       } else {
                           /*error handling*/
                           leave();
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