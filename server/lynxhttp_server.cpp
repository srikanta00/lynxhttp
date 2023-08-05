#include <boost/asio.hpp>

#include "lynxhttp_server.hpp"
#include "lynxhttp_connection.hpp"

namespace net = boost::asio;

class Server::Impl 
{
public:
    Impl() {

    }

    void run(net::ip::tcp::endpoint& endpoint);
    void serve();

private:
    net::io_service ioc;
    shared_ptr<net::ip::tcp::acceptor> acceptor; //{ioc, endpoint};
};

Server::Server() {
    impl_ = new Impl{};
}

Server::~Server() {
    delete impl_;
}

void Server::serve() {
    impl_->serve();
}

void Server::Impl::run(net::ip::tcp::endpoint& ep) {
    auto connection = make_shared<Connection>(ioc);
    acceptor->async_accept(connection->socket(),
                        [this, &connection, &ep](const error_code& ec)
                        {
                            cout << "handler called:" << ec << endl;
                            if(!ec)
                            {
                                connection->run();
                            }

                            this->run(ep);
                        });
}

void Server::Impl::serve() {
    net::ip::tcp::endpoint endpoint{net::ip::tcp::v4(), 8081};
    acceptor = make_shared<net::ip::tcp::acceptor>(ioc, endpoint);
    acceptor->listen();
    run(endpoint);

    ioc.run();
}

