#include <boost/test/unit_test.hpp>

#include <thread>
#include <chrono>

#include "utils.hpp"
#include "lynxhttp_server.hpp"
#include "lynxhttp_message.hpp"

#include "lynxhttp_client.hpp"

namespace lynxserver = lynxhttp::server;
namespace lynxclient = lynxhttp::client;

BOOST_AUTO_TEST_SUITE(test_lynxserver)

BOOST_AUTO_TEST_CASE(server_curl_basic)
{
    BOOST_TEST_MESSAGE("Creating server with default parameters: ");
    lynxserver::server srv;
    
    srv.handle("/", [](const lynxserver::request::ptr req, const lynxserver::response::ptr resp){
        BOOST_TEST_MESSAGE("\nData received: " << req->body());
        BOOST_CHECK_EQUAL(req->body(), "It is a request.");
        resp->end(200, "It is a response.");
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

BOOST_AUTO_TEST_CASE(server_curl_ssl)
{
    BOOST_TEST_MESSAGE("Creating server with ssl enabled: ");
    lynxserver::server srv("0.0.0.0", 443, true);
    srv.set_certificate_chain_file("../examples/certificate.pem");
    srv.set_private_key_file("../examples/key.pem");
    
    srv.handle("/", [](const lynxserver::request::ptr req, const lynxserver::response::ptr resp){
        BOOST_TEST_MESSAGE("\nData received: " << req->body());
        BOOST_CHECK_EQUAL(req->body(), "It is a request.");
        resp->end(200, "It is a response.");
    });

    std::thread thread1([&srv](){
        srv.serve();
    });

    BOOST_TEST_MESSAGE("Waiting for a second...");
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto result = exec("curl -k -X POST \"https://127.0.0.1:443?k1=v1&k2=v2\" -d 'It is a request.'");
    BOOST_TEST_MESSAGE("Response received: " << result);
    BOOST_CHECK_EQUAL(result, "It is a response.");

    BOOST_TEST_MESSAGE("Waiting for a second...");
    std::this_thread::sleep_for(std::chrono::seconds(1));

    srv.stop();
    thread1.join();
    BOOST_TEST_MESSAGE("Test completed.");
}

BOOST_AUTO_TEST_CASE(server_client_basic)
{
    BOOST_TEST_MESSAGE("Creating server with default parameters: ");
    lynxserver::server srv;
    
    srv.handle("/", [](const lynxserver::request::ptr req, const lynxserver::response::ptr resp){
        BOOST_TEST_MESSAGE("Data received: " << req->body());
        if (req->body() == "Bye") {
            resp->end(200, "See you again.");
            return;
        }
        BOOST_CHECK_EQUAL(req->body(), "It is a request.");
        resp->send(200, "It is a response.");
    });

    std::thread thread_server([&srv](){
        srv.serve();
    });

    BOOST_TEST_MESSAGE("Creating client with default parameters: ");
    lynxclient::client clnt;

    auto conn_promise = std::promise<bool>();

    clnt.on_connect([&conn_promise](const boost::system::error_code& ec){
        if (ec) {
            conn_promise.set_value(false);
            return;
        }

        BOOST_TEST_MESSAGE("Connection established: " << ec.message());
        conn_promise.set_value(true);
    });

    clnt.on_close([](const boost::system::error_code ec){
        BOOST_TEST_MESSAGE("Connection closed: " << ec.message());
    });

    std::thread thread_client([&clnt](){
        clnt.run();
    });

    BOOST_TEST_MESSAGE("Waiting for connection to be established...");
    auto conn_future = conn_promise.get_future();
    auto status = conn_future.wait_for(std::chrono::seconds(5));
    if (status == std::future_status::ready) {
        auto b = conn_future.get();
        BOOST_CHECK_EQUAL(b, true);
    } else {
        BOOST_TEST_MESSAGE("Connection not established.");
    }

    BOOST_TEST_MESSAGE("Sending first request...");
    
    auto req = clnt.send("GET", "http://x.com/path1", "It is a request.");

    auto response_promise = std::promise<bool>();
    req->on_response([&response_promise](lynxclient::response::ptr resp){
        BOOST_TEST_MESSAGE("response received: " << resp->body());
        BOOST_CHECK_EQUAL(resp->body(), "It is a response.");
        response_promise.set_value(true);
    });

    BOOST_TEST_MESSAGE("Waiting for a response to reach the client...");
    auto response_future = response_promise.get_future();
    status = response_future.wait_for(std::chrono::seconds(5));
    if (status == std::future_status::ready) {
        auto b = response_future.get();
        BOOST_CHECK_EQUAL(b, true);
    }

    BOOST_TEST_MESSAGE("Sending second request...");
    auto req2 = clnt.send("GET", "http://x.com/path2", "Bye");

    auto response2_promise = std::promise<bool>();
    req2->on_response([&response2_promise](lynxclient::response::ptr resp){
        BOOST_TEST_MESSAGE("response received: " << resp->body());
        BOOST_CHECK_EQUAL(resp->body(), "See you again.");
        response2_promise.set_value(true);
    });

    BOOST_TEST_MESSAGE("Waiting for the second response to reach the client...");
    auto response2_future = response2_promise.get_future();
    status = response2_future.wait_for(std::chrono::seconds(5));
    if (status == std::future_status::ready) {
        auto b = response2_future.get();
        BOOST_CHECK_EQUAL(b, true);
    }

    BOOST_TEST_MESSAGE("Stopping server...");
    srv.stop();
    thread_server.join();

    BOOST_TEST_MESSAGE("Stopping client...");
    clnt.shutdown();
    thread_client.join();

    BOOST_TEST_MESSAGE("Test completed.");
}

BOOST_AUTO_TEST_CASE(server_client_shutdown1)
{
    BOOST_TEST_MESSAGE("Creating server with default parameters: ");
    lynxserver::server srv;
    
    srv.handle("/", [](const lynxserver::request::ptr req, const lynxserver::response::ptr resp){
        BOOST_TEST_MESSAGE("Data received: " << req->body());
        if (req->body() == "Bye") {
            resp->end(200, "See you again.");
            return;
        }
        BOOST_CHECK_EQUAL(req->body(), "It is a request.");
        resp->send(200, "It is a response.");
    });

    std::thread thread_server([&srv](){
        srv.serve();
    });

    BOOST_TEST_MESSAGE("Creating client with default parameters: ");
    lynxclient::client clnt;

    auto conn_promise = std::promise<bool>();

    clnt.on_connect([&conn_promise](const boost::system::error_code& ec){
        if (ec) {
            conn_promise.set_value(false);
            return;
        }

        BOOST_TEST_MESSAGE("Connection established: " << ec.message());
        conn_promise.set_value(true);
    });

    clnt.on_close([](const boost::system::error_code ec){
        BOOST_TEST_MESSAGE("Connection closed: " << ec.message());
    });

    std::thread thread_client([&clnt](){
        clnt.run();
    });

    BOOST_TEST_MESSAGE("Waiting for connection to be established...");
    auto conn_future = conn_promise.get_future();
    auto status = conn_future.wait_for(std::chrono::seconds(5));
    if (status == std::future_status::ready) {
        auto b = conn_future.get();
        BOOST_CHECK_EQUAL(b, true);
    } else {
        BOOST_TEST_MESSAGE("Connection not established.");
    }

    BOOST_TEST_MESSAGE("Stopping server...");
    srv.stop();
    thread_server.join();

    BOOST_TEST_MESSAGE("Stopping client...");
    clnt.shutdown();
    thread_client.join();

    BOOST_TEST_MESSAGE("Test completed.");
}

BOOST_AUTO_TEST_CASE(server_client_shutdown2)
{
    BOOST_TEST_MESSAGE("Creating server with default parameters: ");
    lynxserver::server srv;
    
    srv.handle("/", [](const lynxserver::request::ptr req, const lynxserver::response::ptr resp){
        BOOST_TEST_MESSAGE("Data received: " << req->body());
        if (req->body() == "Bye") {
            resp->end(200, "See you again.");
            return;
        }
        BOOST_CHECK_EQUAL(req->body(), "It is a request.");
        resp->send(200, "It is a response.");
    });

    std::thread thread_server([&srv](){
        srv.serve();
    });

    BOOST_TEST_MESSAGE("Creating client with default parameters: ");
    lynxclient::client clnt;

    auto conn_promise = std::promise<bool>();

    clnt.on_connect([&conn_promise](const boost::system::error_code& ec){
        if (ec) {
            conn_promise.set_value(false);
            return;
        }

        BOOST_TEST_MESSAGE("Connection established: " << ec.message());
        conn_promise.set_value(true);
    });

    clnt.on_close([](const boost::system::error_code ec){
        BOOST_TEST_MESSAGE("Connection closed: " << ec.message());
    });

    std::thread thread_client([&clnt](){
        clnt.run();
    });

    BOOST_TEST_MESSAGE("Waiting for connection to be established...");
    auto conn_future = conn_promise.get_future();
    auto status = conn_future.wait_for(std::chrono::seconds(5));
    if (status == std::future_status::ready) {
        auto b = conn_future.get();
        BOOST_CHECK_EQUAL(b, true);
    } else {
        BOOST_TEST_MESSAGE("Connection not established.");
    }

    BOOST_TEST_MESSAGE("Sending first request...");
    
    auto req = clnt.send("GET", "http://x.com/path1", "It is a request.");

    auto response_promise = std::promise<bool>();
    req->on_response([&response_promise](lynxclient::response::ptr resp){
        BOOST_TEST_MESSAGE("response received: " << resp->body());
        BOOST_CHECK_EQUAL(resp->body(), "It is a response.");
        response_promise.set_value(true);
    });

    BOOST_TEST_MESSAGE("Waiting for a response to reach the client...");
    auto response_future = response_promise.get_future();
    status = response_future.wait_for(std::chrono::seconds(5));
    if (status == std::future_status::ready) {
        auto b = response_future.get();
        BOOST_CHECK_EQUAL(b, true);
    }

    BOOST_TEST_MESSAGE("Stopping server...");
    srv.stop();
    thread_server.join();

    BOOST_TEST_MESSAGE("Stopping client...");
    clnt.shutdown();
    thread_client.join();

    BOOST_TEST_MESSAGE("Test completed.");
}

BOOST_AUTO_TEST_SUITE_END()