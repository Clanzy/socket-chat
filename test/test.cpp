#define BOOST_TEST_MODULE SocketChatTests
#include "message.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(message_test) {
    {
        chat::message msg("test str");
        BOOST_CHECK(msg.length() == msg.deduce_length() && msg.length() == 8);
    }
    {
        chat::message msg("");
        BOOST_CHECK(msg.length() == msg.deduce_length() && msg.length() == 0);
    }
    {
        chat::message msg(chat::message(std::string(1019, 'a')));
        BOOST_CHECK(msg.length() == msg.deduce_length() &&
                    msg.length() == 1019);
    }
    {
        BOOST_CHECK_THROW(chat::message(std::string(1020, 'a')),
                          std::exception);
    }
    {
        BOOST_CHECK_THROW(chat::message(std::string(1021, 'a')),
                          std::exception);
    }
    {
        BOOST_CHECK_THROW(chat::message(std::string(10000, 'a')),
                          std::exception);
    }
}
