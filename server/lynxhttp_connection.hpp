#ifndef __LYNXHTTP_CONNECTION__
#define __LYNXHTTP_CONNECTION__

#include <string>
#include <vector>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/array.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "lynxhttp_message.hpp"
#include "lynxhttp_path_tree.hpp"

namespace net = boost::asio;

namespace lynxhttp {
namespace server {

class socket_pool {
public:
    void add(boost::shared_ptr<net::ip::tcp::socket> socket);
    void add(boost::shared_ptr<net::ssl::stream<net::ip::tcp::socket>> socket);

    void remove(boost::shared_ptr<net::ip::tcp::socket> socket);
    void remove(boost::shared_ptr<net::ssl::stream<net::ip::tcp::socket>> socket);

    void close();

    typedef std::shared_ptr<socket_pool> ptr;

private:
    std::vector<boost::shared_ptr<net::ip::tcp::socket>> sockets_;
    std::vector<boost::shared_ptr<net::ssl::stream<net::ip::tcp::socket>>> ssl_sockets_;
};

class connection : public boost::enable_shared_from_this<connection>
{
public:
    connection(boost::shared_ptr<net::ip::tcp::socket> socket);
    connection(boost::shared_ptr<net::ssl::stream<net::ip::tcp::socket> > socket, net::ssl::context& ssl_context);

    virtual ~connection();

    net::ssl::stream<net::ip::tcp::socket>& ssl_socket();
    net::ip::tcp::socket& socket();

    void run();

    void set_path_tree(path_tree::ptr path_tree);
    void set_socket_pool(socket_pool::ptr socket_pool);

    // request& req();
    void set_req(const std::string& data);

    void handle_request(request::ptr req);
    bool ssl_enabled();

    typedef boost::shared_ptr<connection> Ptr;

    void start_read();
    void handle_read(request::ptr req);

private:
    
    boost::shared_ptr<net::ip::tcp::socket> socket_;
    boost::shared_ptr<net::ssl::stream<net::ip::tcp::socket> > ssl_socket_;
    bool ssl_enabled_;

    boost::array<uint8_t, 8000> data_;

    path_tree::ptr path_tree_;
    socket_pool::ptr socket_pool_;

    // boost::shared_ptr<request> req_;
};

} // namespace server
} // namespace lynxhttp

#endif // __LYNXHTTP_CONNECTION__