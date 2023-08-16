# lynxhttp -  - High Level Asynchronous HTTP Library
A C++ API level abstraction of asynchronous HTTP operations. The server can handle an HTTP request asynchronously and send the response when it is ready. The response can also be sent asynchronously without waiting for the actual delivery.

# HTTP Server
Example of a bare minimum server.
```
#include <lynxhttp_server.hpp>

namespace lynxserver = lynxhttp::server;

int main(int argc, char** argv) {

    if(argc != 3)
    {
        std::cerr
        << "Usage: httpserver <address> <port>\n"
        << "Example:\n"
        << "    httpserver 0.0.0.0 8080\n";
        return EXIT_FAILURE;
    }

    lynxserver::server srv(std::string(argv[1]), static_cast<unsigned short>(std::atoi(argv[2])));

    srv.handle("/", [](const lynxserver::request::ptr req, const lynxserver::response::ptr resp){
        std::cout << "data received: " << req->body() << std::endl;

        resp->send(200, "Hi there. Greetings...\n");
    });

    srv.serve();

    return EXIT_SUCCESS;
}
```

# Make the library
Run `make` command from the `server` directory. A shared object (liblynxhttp.so) will be created in the `Bin` directory. It can be linked with your application.
```
cd server
make
```

# Example
An example server program is availble in the `examples` directory. To make and run that program:
```
cd examples
make
sudo ./httpserver 0.0.0.0 8080
```

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
