#include <lynxhttp_server.hpp>

int main() {

    Server srv;

    srv.handle("/", [](const request& req, const response& resp){
        std::cout << "data received: " << req.data() << std::endl;

        resp.send("Hi there.");
    });
    srv.serve();

    return 0;
}