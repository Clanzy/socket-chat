#define BOOST_TEST_MODULE SocketChatTests
#include "message.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(message_test) {
    {
        chat::message msg("test str");
        BOOST_CHECK(msg.length() == 8);
    }
    {
        chat::message msg("");
        BOOST_CHECK(msg.length() == 0);
    }
    {
        chat::message msg(chat::message(std::string(1023, 'a').data()));
        BOOST_CHECK(msg.length() == 1023);
    }
    {
        BOOST_CHECK_THROW(chat::message(std::string(1024, 'a').data()),
                          std::exception);
    }
    {
        BOOST_CHECK_THROW(chat::message(std::string(1025, 'a').data()),
                          std::exception);
    }
    {
        BOOST_CHECK_THROW(chat::message(std::string(10000, 'a').data()),
                          std::exception);
    }
}
