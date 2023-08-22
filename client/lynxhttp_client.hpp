#ifndef __LYNXHTTP_CLIENT__
#define __LYNXHTTP_CLIENT__

#include <string>

#include "lynxhttp_client_message.hpp"

namespace lynxhttp {
namespace client {

// typedef void(*connection_cb)(const boost::system::error_code& ec);
typedef std::function<void(const boost::system::error_code& ec)> connection_cb;

class client {
public:
    client(std::string host = "localhost", unsigned short port = 80, int n_threads = 2, bool ssl_enabled = false);
    virtual ~client();

    void run();
    void on_connect(connection_cb cb);
    void stop();

    request::ptr send(const std::string& method = "GET", const std::string& url = "/", const std::string& data = "");

private:
    class Impl;
    Impl* impl_;
};

} // namespace client
} // namespace lynxhttp

#endif // __LYNXHTTP_CLIENT__