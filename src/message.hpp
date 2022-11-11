#pragma once

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

namespace chat {

class message {
public:
    /*first 4 bytes encode length from 0 to 1020*/
    static constexpr size_t length_bytes = 4;
    /*then goes 1020 bytes that contain the message*/
    static constexpr size_t message_bytes = 1019;

    static constexpr size_t full_length = length_bytes + message_bytes;

private:
    char full_message[full_length];
    size_t body_length;

public:
    message() : body_length{0} {}
    message(const std::string &str) : body_length{0} { encode(str); }

    size_t length() const { return body_length; }

    char *data() { return full_message; }
    char *msg() { return full_message + length_bytes; }

    void encode(const std::string &str) {
        body_length = str.length();
        if (body_length > message_bytes) {
            throw std::runtime_error("message is too long");
        }
        std::string len = std::to_string(body_length);
        std::memset(full_message, 0, length_bytes);
        std::memcpy(full_message, len.data(), length_bytes);
        std::memcpy(full_message + length_bytes, str.data(), message_bytes);
    }

    /**
     * this function should be called after receiving the message to set member
     * variable body_length to corresponding length (otherwise it will be 0)
     *
     * it also checked if received message is valid (deduced length lower or
     * equal than maximal possible)
     */
    bool is_valid_message() {
        /*function sets body_length automaticly so no need to do it manually*/
        body_length = deduce_length();
        return body_length <= message_bytes;
    }
    /**
     * reads first bytes and deduces length of message from them
     *
     * function is similar to length() getter, but instead gets length from
     * first bytes of array
     */
    size_t deduce_length() {
        std::string prefix(full_message, length_bytes);
        size_t len;
        std::stringstream(prefix) >> len;
        return len;
    }
};

} // namespace chat