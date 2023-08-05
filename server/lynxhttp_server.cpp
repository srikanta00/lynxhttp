#include <boost/asio.hpp>

#include "lynxhttp_server.hpp"


namespace net = boost::asio;

class Server::Impl 
{
public:
    Impl() {

    }

    void run(net::ip::tcp::endpoint& endpoint);
    void serve();

    void set_callback(cb callback);
private:
    net::io_service ioc;
    shared_ptr<net::ip::tcp::acceptor> acceptor; //{ioc, endpoint};

    cb cb_;
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

void Server::handle(const std::string& path, cb callback) {
    impl_->set_callback(callback);
}

void Server::Impl::run(net::ip::tcp::endpoint& ep) {
    /* std::make_shared did not work with shared_from_this(). Don't know why. */
    auto connection = boost::shared_ptr<Connection>(new Connection(ioc));

    connection->set_callback(cb_);

    acceptor->async_accept(connection->socket(),
                        [this, connection, &ep](const error_code& ec)
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

void Server::Impl::set_callback(cb callback) {
    cb_ = callback;
}