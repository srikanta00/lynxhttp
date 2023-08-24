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
        }
        resp->send(200, "Hi there. Greetings...\n");
    });
    
    srv.serve();

    return EXIT_SUCCESS;
}
