#pragma once

#include <array>
#include <cstring>
#include <stdexcept>
#include <string>

namespace chat {

enum class Action : char { USERNAME, MESSAGE, MAX_ACTIONS };
class request {
private:
    static constexpr size_t length_          = 1024;
    static constexpr size_t action_length_   = 1;
    static constexpr size_t username_length_ = 20;  // 21 with null-terminator
    static constexpr size_t message_length_ = 1020; // 1021 with null-terminator
    std::array<char, length_> request_;

public:
    const char *data() const { return request_.data(); }
    char *data() { return request_.data(); }
    constexpr size_t length() const { return length_; }
    constexpr size_t username_length() const { return username_length_; }
    constexpr size_t message_length() const { return message_length_; }

    void encode_action(Action action) {
        *request_.data() = static_cast<char>(action);
    }
    Action action() const { return static_cast<Action>(*request_.data()); }

    void encode_username(std::string username) {
        std::memset(request_.data() + action_length_, 0, username_length_ + 1);
        if (username.length() > username_length_) {
            throw std::runtime_error("Username is too long");
        }
        if (username.length() == 0) {
            throw std::runtime_error("Username is too short");
        }
        std::memcpy(request_.data() + action_length_, username.data(),
                    username.length() + 1);
    }
    const char *username() const { return request_.data() + action_length_; }

    void encode_message(std::string message) {
        std::memset(request_.data() + action_length_ + username_length_ + 1, 0,
                    message_length_);
        if (message.length() > message_length_) {
            throw std::runtime_error("Message is too long");
        }
        std::memcpy(request_.data() + action_length_ + username_length_ + 1,
                    message.data(), message.length() + 1);
    }
    const char *message() const {
        return request_.data() + action_length_ + username_length_ + 1;
    }
};

}; // namespace chat