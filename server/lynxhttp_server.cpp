#include <boost/asio.hpp>

#include "lynxhttp_server.hpp"


namespace net = boost::asio;

class server::Impl 
{
public:
    Impl(std::string address = "", unsigned short port = 80) {
        address_ = address;
        port_ = port;
    }

    void run(net::ip::tcp::endpoint& endpoint);
    void serve();

    void set_callback(cb callback);
private:
    net::io_service ioc;
    shared_ptr<net::ip::tcp::acceptor> acceptor; //{ioc, endpoint};
    std::string address_;
    unsigned short port_;
    cb cb_;
};

server::server(std::string address, unsigned short port) {
    impl_ = new Impl(address, port);
}

server::~server() {
    delete impl_;
}

void server::serve() {
    impl_->serve();
}

void server::handle(const std::string& path, cb callback) {
    impl_->set_callback(callback);
}

void server::Impl::run(net::ip::tcp::endpoint& ep) {
    /* std::make_shared did not work with shared_from_this(). Don't know why. */
    auto conn = boost::shared_ptr<connection>(new connection(ioc));

    conn->set_callback(cb_);

    acceptor->async_accept(conn->socket(),
                        [this, conn, &ep](const error_code& ec)
                        {
                            cout << "handler called:" << ec << endl;
                            if(!ec)
                            {
                                conn->run();
                            }

                            this->run(ep);
                        });
}

void server::Impl::serve() {
    auto address = net::ip::make_address(address_);
    // net::ip::tcp::endpoint endpoint{net::ip::tcp::v4(), port_};
    net::ip::tcp::endpoint endpoint{address, port_};
    acceptor = make_shared<net::ip::tcp::acceptor>(ioc, endpoint);
    acceptor->listen();
    run(endpoint);
    
    ioc.run();
}

void server::Impl::set_callback(cb callback) {
    cb_ = callback;
}