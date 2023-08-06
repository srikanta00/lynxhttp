#include <iostream>

#include "lynxhttp_connection.hpp"

connection::connection(net::io_service& ios) :
            socket_(ios) {

    req_ = boost::shared_ptr<request>(new request());
}

connection::~connection() {
    std::cout << "connection::destructor called" << std::endl;
}

net::ip::tcp::socket& connection::socket() {
    return socket_;
}

void connection::run() {
    socket_.async_read_some(boost::asio::buffer(data_), 
        [sp = shared_from_this()](const boost::system::error_code& err,
        std::size_t bytes_transferred){
            
            /*TODO: avoid memory copy.*/
            sp->set_req(std::string(sp->data_.begin(), sp->data_.end()));
            sp->handle_request();
        }
        );
}

void connection::set_callback(cb callback) {
    cb_ = callback;
}

request& connection::req() {
    return *req_;
}

void connection::set_req(const std::string& data) {
    req_->set_data(data);
}

void connection::handle_request() {
    req_->parse();
    auto resp = boost::shared_ptr<response>(new response());
    resp->set_connection(shared_from_this());
    cb_(req_, resp);
}