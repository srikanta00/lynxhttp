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


int request::parse() {
    int index = 0;
    int len = 0;
    int i, l, s;

    i = data_.find("\r\n", index);
    if (i == std::string::npos) {
        return -1;
    }

    auto request_line = data_.substr(index, i - index);
    index = i + 2;

    s = 0;
    i = request_line.find(" ", s);
    auto method = request_line.substr(s, i - s);
    header_["method"] = method;

    s = request_line.find_first_not_of(" ", i);
    i = request_line.find(" ", s);
    auto path = request_line.substr(s, i - s);
    header_["path"] = path;

    s = request_line.find("/", i);
    auto version = request_line.substr(s+1);
    header_["version"] = method;

    while(((i = data_.find("\r\n", index)) - index) > 0) {
        auto header_line = data_.substr(index, i - index);
        index = i + 2;

        i = header_line.find(":");
        auto key = header_line.substr(0, i);

        s = header_line.find_first_not_of(" ", i + 1);
        auto value = header_line.substr(s);

        header_[key] = value;
    }
    
    index += 2;

    body_ = data_.substr(index);

    return 0;
}

const std::string& request::body() const {
    return body_;
}

/////////////////////////////////////////////////////////////////////////////////////////
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

void response::set_connection(const connection::Ptr& conn) {
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
