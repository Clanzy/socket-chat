#pragma once

#include <cstring>
#include <iostream>
#include <string>

namespace chat {

/**
 * Wrapper over cstring
 *
 * This class has fixed length that is known by both server and client and its
 * function length() calls strlen() if current length is zero.
 * 
 * It allows server and client send and receive underlying array and then print it.
 */
class message {
public:
    static constexpr size_t capacity = 1024;

private:
    char message_[capacity];
    mutable size_t length_;

public:
    message() : length_{0} {}

    message(const char *str) {
        std::memset(message_, 0, capacity);
        if (str) { // check for NULL
            length_ = std::strlen(str);
            if (length_ + 1 > capacity) {
                throw std::runtime_error("Message is too long");
            }
            std::memcpy(message_, str, length_ + 1);
        } else {
            length_     = 0;
            message_[0] = '\0';
        }
    }

    size_t length() const {
        if (length_ == 0) {
            deduce_length();
        }
        return length_;
    }

    char *data() { return message_; }
    const char *data() const { return message_; }

private:
    void deduce_length() const {
        if (message_[0] != '\0') {
            length_ = std::strlen(message_);
        } else {
            length_ = 0;
        }
    };
};

}; // namespace chat