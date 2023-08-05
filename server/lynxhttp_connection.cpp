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
    auto sp = shared_from_this();
    socket_.async_read_some(boost::asio::buffer(data_), 
        [sp](const boost::system::error_code& err,
        std::size_t bytes_transferred){

            sp->set_req(std::string(sp->data_.begin(), sp->data_.end()));
            sp->resp().set_connection(sp);
            sp->handle_request();
        }
        );
}

void Connection::set_callback(cb callback) {
    cb_ = callback;
}

request& Connection::req() {
    return req_;
}

void Connection::set_req(const std::string& data) {
    req_.set_data(data);
}

response& Connection::resp() {
    return resp_;
}

void Connection::set_resp(const std::string& data) {
    resp_.set_data(data);
}

void Connection::handle_request() {
    cb_(req_, resp_);
}