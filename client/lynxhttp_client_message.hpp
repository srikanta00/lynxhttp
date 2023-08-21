#ifndef __LYNXHTTP_CLIENT_MESSAGE__
#define __LYNXHTTP_CLIENT_MESSAGE__

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace lynxhttp {
namespace client {

class response {
public:
    response();
    virtual ~response();

    void append_data(const std::string& data);
    std::string body();
    
    enum parsing_state_t {
        EMPTY,
        INCOMPLETE,
        COMPLETE,
        ERROR
    };

    parsing_state_t parse();
    parsing_state_t parsing_state();

    typedef boost::shared_ptr<response> ptr;

private:
    std::map<std::string, std::string> header_;
    std::string body_;

    std::string data_;
    parsing_state_t parsing_state_;
    int parsed_len_;
};

typedef void(*response_cb)(const response::ptr resp);

class request : public boost::enable_shared_from_this<request>
{
public:
    request(std::string method = "GET", std::string path = "/", const std::string& body = "");
    virtual ~request();
    std::string body() const;
    void set_body(const std::string& data);

    std::string serialize();

    typedef boost::shared_ptr<request> ptr;

    void on_response(response_cb cb);
    response_cb get_response_cb();

private:
    std::map<std::string, std::string> header_;
    std::string body_;

    std::string method_;
    std::string path_;

    response_cb cb_;
};

} // namespace client
} // namespace lynxhttp

#endif // __LYNXHTTP_CLIENT_MESSAGE__