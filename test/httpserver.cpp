#include <lynxhttp_server.hpp>

int main(int argc, char** argv) {

    if(argc != 3)
    {
        std::cerr
        << "Usage: httpserver <address> <port>\n"
        << "Example:\n"
        << "    httpserver 0.0.0.0 8080\n";
        return EXIT_FAILURE;
    }

    server srv(std::string(argv[1]), static_cast<unsigned short>(std::atoi(argv[2])));

    srv.handle("/", [](const request::ptr req, const response::ptr resp){
        std::cout << "data received: " << req->body() << std::endl;

        resp->send("Hi there. Greetings...");
    });

    srv.serve();

    return EXIT_SUCCESS;
}