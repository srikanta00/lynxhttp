#include <lynxhttp_server.hpp>

int main() {

    server srv;

    srv.handle("/", [](const request::ptr req, const response::ptr resp){
        std::cout << "data received: " << req->body() << std::endl;

        resp->send("Hi there. Greetings...");
    });
    srv.serve();

    return 0;
}