#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE lynxhttp_test

#include <boost/test/unit_test.hpp>

#include <thread>
#include <chrono>

#include "utils.hpp"
#include "lynxhttp_server.hpp"
#include "lynxhttp_message.hpp"

namespace lynxserver = lynxhttp::server;

BOOST_AUTO_TEST_SUITE(test_lynxserver)

BOOST_AUTO_TEST_CASE(server_curl_basic)
{
    BOOST_TEST_MESSAGE("Creating server with default parameters: ");
    lynxserver::server srv;
    
    srv.handle("/", [](const lynxserver::request::ptr req, const lynxserver::response::ptr resp){
        BOOST_TEST_MESSAGE("Data received: " << req->body());
        BOOST_CHECK_EQUAL(req->body(), "It is a request.");
        resp->send(200, "It is a response.");
    });

    std::thread thread1([&srv](){
        srv.serve();
    });

    BOOST_TEST_MESSAGE("Waiting for a second...");
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto result = exec("curl -X POST \"127.0.0.1:80?k1=v1&k2=v2\" -d 'It is a request.'");
    BOOST_TEST_MESSAGE("Response received: " << result);
    BOOST_CHECK_EQUAL(result, "It is a response.");

    BOOST_TEST_MESSAGE("Waiting for a second...");
    std::this_thread::sleep_for(std::chrono::seconds(1));

    srv.stop();
    thread1.join();
    BOOST_TEST_MESSAGE("Test completed.");
}

BOOST_AUTO_TEST_SUITE_END()