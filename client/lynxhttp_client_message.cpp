#include "lynxhttp_client_message.hpp"

namespace lynxhttp {
namespace client {

request::request(std::string method, std::string path) :
method_(method), path_(path) {

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

    request = request + method_ + " " + path_ + "HTTP/1.1\r\n";
    request.append("User-Agent: lynxhttp-client\r\n");

    int len = body_.length();
    request.append("Content-Length: " + std::to_string(len) + "\r\n");

    if (len > 0) {
        request.append("\r\n");
        request.append(body_);
    }

    return request;
}

//////////////////////////////////////////////////////////////////////////

response::response() {

}

response::~response() {

}

} // namespace client
} // namespace lynxhttp