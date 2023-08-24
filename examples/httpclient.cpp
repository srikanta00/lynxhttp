#include <iostream>

#include <boost/asio.hpp>

#include "lynxhttp_client.hpp"

namespace lynxclient = lynxhttp::client;

int main(int argc, char** argv) {

    if(argc < 3)
    {
        std::cerr
        << "Usage: httpclient <address> <port>\n"
        << "Example:\n"
        << "    httpclient 12.0.0.1 8080\n";
        return EXIT_FAILURE;
    }

    lynxclient::client clnt(std::string(argv[1]), static_cast<unsigned short>(std::atoi(argv[2])));

    clnt.on_connect([&clnt](const boost::system::error_code& ec){
        auto req = clnt.send("GET", "http://127.0.0.1:8080/", "Hello");

        req->on_response([&clnt](lynxclient::response::ptr resp) {
            std::cout << "response received: " << resp->body() << std::endl;

            /*Sending the second resonse on the same connection.*/
            auto req2 = clnt.send("GET", "http://127.0.0.1:8080/", "Bye");
            req2->on_response([](lynxclient::response::ptr resp) {
                std::cout << "response received: " << resp->body() << std::endl;
            });
        });
    });

    auto close_promise = std::promise<bool>();
    clnt.on_close([&close_promise](const boost::system::error_code ec){
        close_promise.set_value(true);
    });

    std::thread t1([&clnt]() {
        clnt.run();
    });

    auto close_future = close_promise.get_future();
    if (close_future.get()) clnt.shutdown();
    
    t1.join();

    return 0;
}