#include "lynxhttp_client_message.hpp"
#include <iostream>
namespace lynxhttp {
namespace client {

request::request(std::string method, std::string path, const std::string& body) :
method_(method), path_(path), body_(body) {

}

request::~request() {

}
    
std::string request::body() const {
    return body_;
}

void request::set_body(const std::string& data) {
    body_ = data;
}

std::string request::serialize() {
    std::string request;

    request = request + method_ + " " + path_ + " HTTP/1.1\r\n";
    request.append("User-Agent: lynxhttp-client\r\n");

    int len = body_.length();
    request.append("Content-Length: " + std::to_string(len) + "\r\n");

    if (len > 0) {
        request.append("\r\n");
        request.append(body_);
    }

    return request;
}

void request::on_response(response_cb cb) {
    cb_ = cb;
}

response_cb request::get_response_cb() {
    return cb_;
}

void request::set_resp(response::ptr resp) {
    resp_ = resp;
}

response::ptr request::resp() {
    return resp_;
}

//////////////////////////////////////////////////////////////////////////

response::response() {
    parsed_len_ = 0;
    parsing_state_ = EMPTY;
}

response::~response() {

}

void response::append_data(const std::string& data) {
    data_.append(data);
}

response::parsing_state_t response::parse() {
    int index = parsed_len_;
    int i, l, s;

    if (parsing_state_ == EMPTY) {
        i = data_.find("\r\n", index);
        if (i == std::string::npos) {
            return EMPTY;
        }

        auto response_line = data_.substr(index, i - index);

        index = i + 2;

        s = 0;
        i = response_line.find(" ", s);
        auto http_ver = response_line.substr(s, i - s);

        s = http_ver.find("/");
        auto version = http_ver.substr(s + 1);

        header_["version"] = version;

        s = response_line.find_first_not_of(" ", i);
        i = response_line.find(" ", s);
        auto status = response_line.substr(s, i - s);
        header_["status"] = status;

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

    int data_len = data_.length() - index;

    if (data_len < len) {
        parsing_state_ = INCOMPLETE;
        return INCOMPLETE;
    }

    if (data_len >= len) {
        body_ = data_.substr(index, len);
        parsed_len_ = index + len;
        parsing_state_ = COMPLETE;
        return COMPLETE;
    }

    parsing_state_ = ERROR;
    return ERROR;
}

std::string response::extra_data() {
    if (data_.length() > parsed_len_) return data_.substr(parsed_len_);

    return std::string();
}

response::parsing_state_t response::parsing_state() {
    return parsing_state_;
}

std::string response::body() {
    return body_;
}

} // namespace client
} // namespace lynxhttp