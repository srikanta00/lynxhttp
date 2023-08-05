#include "lynxhttp_message.hpp"
#include "lynxhttp_connection.hpp"

#include <iostream>

request::request() {

}

request::~request() {

}

std::string request::data() const {
    return data_;
}

void request::set_data(const std::string& data) {
    data_ = data;
}

response::response() {

}

response::~response() {

}

std::string response::data() const {
    return data_;
}

void response::set_data(const std::string& data) {
    data_ = data;
}

void response::set_connection(const Connection::Ptr& conn) {
    conn_ = conn;
}

void response::send(const std::string& resp) const {
    std::cout << "response send called" << std::endl;

    conn_->socket().async_write_some(boost::asio::buffer(resp),
        [](const boost::system::error_code& err,
                        std::size_t bytes_transferred) {
            std::cout << "Message sent" << std::endl;
        }
    );

}