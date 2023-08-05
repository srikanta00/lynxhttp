#include <lynxhttp_server.hpp>

int main() {

    server srv;

    srv.handle("/", [](const request::ptr req, const response::ptr resp){
        std::cout << "data received: " << req->data() << std::endl;

        resp->send("Hi there.");
    });
    srv.serve();

    return 0;
}