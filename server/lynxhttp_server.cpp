#include <lynxhttp_server.hpp>

server::server() {
    cout << "constructor" << endl;
}

server::~server() {
    cout << "destructor" << endl;
}

void server::identity() {
    cout << "identity" << endl;
}

void server::run(net::ip::tcp::endpoint& ep) {
    auto socket = make_shared<net::ip::tcp::socket>(ioc);
    acceptor->async_accept(*socket,
                        [this, socket, &ep](const error_code& ec)
                        {
                            cout << "handler called:" << ec << ":" << this->tmp++ << endl;
                            if(!ec)
                            {
                                cout << "data received" << endl;
                                exit(0);
                            }

                            this->run(ep);
                        });
}

void server::serve() {
    net::ip::tcp::endpoint endpoint{net::ip::tcp::v4(), 8081};
    acceptor = make_shared<net::ip::tcp::acceptor>(ioc, endpoint);
    acceptor->listen();
    run(endpoint);

    ioc.run();
}