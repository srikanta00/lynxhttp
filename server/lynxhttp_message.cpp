#include "lynxhttp_message.hpp"
#include "lynxhttp_connection.hpp"
#include "lynxhttp_response_status_code.hpp"

#include <iostream>

request::request() {
    parsing_state_ = EMPTY;
    parsed_len_ = 0;
}

request::~request() {

}

std::string request::data() const {
    return data_;
}

void request::set_data(const std::string& data) {
    data_ = data;
}

void request::append_data(const std::string& data) {
    data_ = data_ + data;
}

request::parsing_state_t request::parsing_state() {
    return parsing_state_;
}

request::parsing_state_t request::parse() {
    int index = parsed_len_;
    int i, l, s;

    if (parsing_state_ == EMPTY) {
        i = data_.find("\r\n", index);
        if (i == std::string::npos) {
            return EMPTY;
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

        parsing_state_ = INCOMPLETE;
    }
    
    parsed_len_ = index;

    while(((i = data_.find("\r\n", index)) - index) > 0) {

        auto header_line = data_.substr(index, i - index);
        index = i + 2;

        i = header_line.find(":");
        if (i == std::string::npos) return parsing_state_;
        auto key = header_line.substr(0, i);

        s = header_line.find_first_not_of(" ", i + 1);
        if (s == std::string::npos) return parsing_state_;
        auto value = header_line.substr(s);

        header_[key] = value;
        parsed_len_ = index;
    }
    
    index += 2;

    if (header_.find("Content-Length") == header_.end()) {
        return parsing_state_;
    }

    int len = std::stoi(header_["Content-Length"]);

    if ((data_.length() - index) < len) {
        parsing_state_ = INCOMPLETE;
        return INCOMPLETE;
    }

    if ((data_.length() - index) == len) {
        body_ = data_.substr(index);
        parsed_len_ = data_.length();
        parsing_state_ = COMPLETE;
        return COMPLETE;
    }

    parsing_state_ = ERROR;
    return ERROR;
}

const std::map<std::string, std::string> request::header() const {
    return header_;
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

std::string response::serialize() {
    std::string response;

    response = "HTTP/" + header_["version"] + " " + response_status_code::to_string(status_code_) +"\r\n";

    for (const auto& [key, value]: header_) {
        if (key == "version" || key == "path" || key == "version") continue;
        response = response + key + ": " + value + "\r\n";
    }

    if (body_.length()) {
        response = response + "\r\n";
        response = response + body_;
    }

    return response;
}

void response::send(const std::string& resp) {
    send(200, resp);
}

void response::send(int status_code, const std::string& resp) {
    auto header = conn_->req().header();
    status_code_ = status_code;
    header_["version"] = header["version"];
    header_["Content-Length"] = std::to_string(resp.length());
    body_ = resp;

    if(conn_->ssl_enabled()) {
        conn_->ssl_socket().async_write_some(boost::asio::buffer(serialize()),
            [sp = shared_from_this()](const boost::system::error_code& err,
                            std::size_t bytes_transferred) {
                // std::cout << "Message sent" << std::endl;
            }
        );
    } else {
        conn_->socket().async_write_some(boost::asio::buffer(serialize()),
            [sp = shared_from_this()](const boost::system::error_code& err,
                            std::size_t bytes_transferred) {
                // std::cout << "Message sent" << std::endl;
            }
        );
    }
}