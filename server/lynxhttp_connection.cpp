#include <iostream>
#include <boost/bind.hpp>

#include "lynxhttp_connection.hpp"

Connection::Connection(net::io_service& ios) :
            socket_(ios) {

}

Connection::~Connection() {

}

net::ip::tcp::socket& Connection::socket() {
    return socket_;
}

void Connection::run() {
    socket_.async_read_some(boost::asio::buffer(data_), 
                            boost::bind(&Connection::handle_read, shared_from_this(),
                net::placeholders::error, net::placeholders::bytes_transferred));
}

void Connection::handle_read(const boost::system::error_code& err,
            std::size_t bytes_transferred) {
    std::cout << "data read: " << std::string(data_.begin(), data_.end()) << std::endl;
}