#ifndef __LYNXHTTP_CLIENT_MESSAGE__
#define __LYNXHTTP_CLIENT_MESSAGE__

#include <string>
#include <map>

namespace lynxhttp {
namespace client {

class request {
public:
    request(std::string method = "GET", std::string path = "/");
    virtual ~request();
    std::string body() const;
    void set_body(const std::string& data);

    std::string serialize();

private:
    std::map<std::string, std::string> header_;
    std::string body_;

    std::string method_;
    std::string path_;
};

class response {
public:
    response();
    virtual ~response();

private:
    int a;
};

} // namespace client
} // namespace lynxhttp

#endif // __LYNXHTTP_CLIENT_MESSAGE__