#define BOOST_TEST_MODULE SocketChatTests
#include "request.hpp"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(message_test) {
    
    {
        chat::request req;
        req.encode_username("Clanzy");
        BOOST_CHECK(strcmp(req.username(), "Clanzy") == 0);
    }
    {
        chat::request req;
        BOOST_CHECK_THROW(req.encode_username(""), std::exception);
    }
    {
        chat::request req;
        BOOST_CHECK_THROW(req.encode_username("Username is 21 charss"), std::exception);
    }
    {
        chat::request req;
        BOOST_CHECK_NO_THROW(req.encode_username("Username is 20 chars"));
    }
    {
        chat::request req;
        req.encode_message("Informative message");
        BOOST_CHECK(strcmp(req.message(), "Informative message") == 0);
    }
    {
        chat::request req;
        BOOST_CHECK_THROW(req.encode_message(std::string(1021, 'c')), std::exception);
    }
    {
        chat::request req;
        BOOST_CHECK_NO_THROW(req.encode_message(std::string(1020, 'c')));
    }
    {
        chat::request req;
        req.encode_message("");
        BOOST_CHECK(strcmp(req.message(), "") == 0);
    }
    
}
