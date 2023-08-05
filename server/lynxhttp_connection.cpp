#include <iostream>

#include "lynxhttp_connection.hpp"

Connection::Connection(net::io_service& ios) :
            socket_(ios) {

}

Connection::~Connection() {
    std::cout << "connection::destructor called" << std::endl;
}

net::ip::tcp::socket& Connection::socket() {
    return socket_;
}

void Connection::run() {
    socket_.async_read_some(boost::asio::buffer(data_), 
                            [sp = shared_from_this()](const boost::system::error_code& err,
            std::size_t bytes_transferred){
                                std::cout << "data read: " << std::string(sp->data_.begin(), sp->data_.end()) << std::endl;

                                std::string msg("Hi there! I am not ready");
                                sp->socket().async_write_some(boost::asio::buffer(msg),
                                    [sp](const boost::system::error_code& err,
                                                    std::size_t bytes_transferred) {
                                        std::cout << "Message sent" << std::endl;
                                    }
                                );
                            
                            });
}