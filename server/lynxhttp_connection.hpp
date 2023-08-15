#ifndef __LYNXHTTP_CONNECTION__
#define __LYNXHTTP_CONNECTION__

#include<string>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/array.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "lynxhttp_message.hpp"
#include "lynxhttp_path_tree.hpp"

namespace net = boost::asio;

namespace lynxhttp {
namespace server {

class connection : public boost::enable_shared_from_this<connection>
{
public:
    connection(net::ip::tcp::socket socket);
    connection(net::ip::tcp::socket socket, net::ssl::context& ssl_context);

    virtual ~connection();

    net::ssl::stream<net::ip::tcp::socket>& ssl_socket();
    net::ip::tcp::socket& socket();

    void run();

    void set_path_tree(path_tree::ptr path_tree);

    request& req();
    void set_req(const std::string& data);

    void handle_request();
    bool ssl_enabled();

    typedef boost::shared_ptr<connection> Ptr;

private:
    void handle_read();

    boost::shared_ptr<net::ip::tcp::socket> socket_;
    boost::shared_ptr<net::ssl::stream<net::ip::tcp::socket> > ssl_socket_;
    bool ssl_enabled_;

    boost::array<uint8_t, 8000> data_;

    path_tree::ptr path_tree_;
    boost::shared_ptr<request> req_;
};

} // namespace server
} // namespace lynxhttp

#endif // __LYNXHTTP_CONNECTION__