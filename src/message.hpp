#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

class message {
public:
    static constexpr size_t length_bytes  = 4;
    static constexpr size_t message_bytes = 1020;
    static constexpr size_t full_length   = length_bytes + message_bytes;

private:
    char full_message[full_length];
    size_t body_length;

public:
    message() : body_length{0} {}
    message(std::string &str) : body_length{0} { encode(str); }

    size_t length() const { return body_length; }

    char *data() { return full_message; }
    char *msg() { return full_message + length_bytes; }

    void encode(std::string &str) {
        body_length     = str.length();
        std::string len = std::to_string(body_length);
        std::memset(full_message, 0, length_bytes);
        std::memcpy(full_message, len.data(), length_bytes);
        std::memcpy(full_message + length_bytes, str.data(), message_bytes);
    }

    bool is_valid_message() {
        body_length = deduce_length();
        return body_length < message_bytes;
    }

    size_t deduce_length() {
        std::string prefix(full_message, length_bytes);
        size_t len;
        std::stringstream(prefix) >> len;
        return len;
    }
};