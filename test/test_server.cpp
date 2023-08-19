#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE Hello
#include <boost/test/unit_test.hpp>

#include "lynxhttp_server.hpp"
#include "lynxhttp_message.hpp"

namespace lynxserver = lynxhttp::server;

int add(int i, int j)
{
    return i + j;
}

BOOST_AUTO_TEST_SUITE(test_lynxserver)

BOOST_AUTO_TEST_CASE(test1)
{
    BOOST_TEST_MESSAGE("Creating server with default parameters: ");
    lynxserver::server srv;
    srv.handle("/", [](const lynxserver::request::ptr req, const lynxserver::response::ptr resp){
        std::cout << "cout:Data received: " << req->body() << std::endl;
        BOOST_TEST_MESSAGE("Data received: " << req->body());
        resp->send(200, "response\n");
    });

    srv.serve();

    BOOST_CHECK(add(2, 2) == 4);
}

BOOST_AUTO_TEST_SUITE_END()