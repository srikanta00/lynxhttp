# lynxhttp -  - High Level Asynchronous HTTP Library
A C++ API level abstraction of asynchronous HTTP client-server operations. The server can handle an HTTP request asynchronously and send the response when it is ready. The response can also be sent asynchronously without waiting for the actual delivery.
The client can connect to an HTTP server and send request asyncrounouly. The notification callbacks get called with the connection is established or a response comes.

# Make the libraries
To make the server library, run `make` command from the `server` directory. A shared object (liblynxhttp.so) will be created in the `Bin` directory. It can be linked with your application.
```
cd server
make
```

To make the client library, run `make` command from the `client` directory. A shared object (liblynxhttpclient.so) will be created in the `Bin` directory. It can be linked with your application.
```
cd server
make
```

# Examples
The example server and client programs are availble in the `examples` directory.
## Example Server
A simple HTTP server:
```
#include <lynxhttp_server.hpp>

namespace lynxserver = lynxhttp::server;

int main(int argc, char** argv) {

    if(argc < 3)
    {
        std::cerr
        << "Usage: httpserver <address> <port>\n"
        << "Example:\n"
        << "    httpserver 0.0.0.0 8080\n";
        return EXIT_FAILURE;
    }

    bool ssl_enabled = false;

    if(argc > 3) {
        if(static_cast<unsigned short>(std::atoi(argv[3])) == 1) ssl_enabled = true;
    }

    lynxserver::server srv(std::string(argv[1]), static_cast<unsigned short>(std::atoi(argv[2])), ssl_enabled);

    if (ssl_enabled) {
        srv.set_certificate_chain_file("certificate.pem");
        srv.set_private_key_file("key.pem");
    }

    srv.handle("/", [](const lynxserver::request::ptr req, const lynxserver::response::ptr resp){

        std::cout << "/: data received: " << req->body() << std::endl;
        if (req->body() == "Bye") {
            resp->end(200, "See you again...");
            return;
        }
        
        resp->send(200, "Hi there. Greetings...\n");
    });
    
    srv.serve();

    return EXIT_SUCCESS;
}
```
## Example Client
A simple HTTP client:
```
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
        << "    httpclient 127.0.0.1 8080\n";
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

    return EXIT_SUCCESS;
}
```
## Build and run the examples
To make the example client server programs:
```
cd examples
make
```
## Run the server
```
sudo ./httpserver 0.0.0.0 8080
Server started.
/: data received: Hello
/: data received: Bye
```
## Run the client
```
./httpclient 127.0.0.1 8080
Connecting 127.0.0.1:8080...
Connected to 127.0.0.1:8080
response received: Hi there. Greetings...
response received: See you again...
```

# Testing with curl
Test the server with the curl command.
```
curl -X POST 127.0.0.1:8080 -d 'hello'
  Hi there. Greetings...
```

Start the server with SSL enabled.
```
./httpserver 0.0.0.0 8081 1
```
Test using curl command.
```
curl -k -X POST https://127.0.0.1:8081 -d 'hello'
    Hi there. Greetings...
```
