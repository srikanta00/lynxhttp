#include <boost/asio.hpp>

#include "lynxhttp_client.hpp"

namespace net = boost::asio;

namespace lynxhttp {
namespace client {

class client::Impl {
public:
    Impl(std::string host, unsigned short port, int n_threads, bool ssl_enabled) :
    host_(host), port_(port), n_threads_(n_threads), ssl_enabled_(ssl_enabled_) {

    }

    virtual ~Impl() {
    }

private:
    net::io_service ios_;
    int n_threads_;

    std::string host_;
    unsigned short port_;
    bool ssl_enabled_;
};

client::client(std::string host, unsigned short port, int n_threads, bool ssl_enabled) {
    impl_ = new Impl(host, port, n_threads, ssl_enabled);
}

client::~client() {
    if (impl_) delete impl_;
}

//////////////////////////////////////////////////////////////////////////////////////

} // namespace client
} // namespace lynxhttp