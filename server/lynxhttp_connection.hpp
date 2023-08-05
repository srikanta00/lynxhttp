#ifndef __LYNXHTTP_CONNECTION__
#define __LYNXHTTP_CONNECTION__

#include<string>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "lynxhttp_message.hpp"

namespace net = boost::asio;

typedef void(*cb)(const request& req, const response& resp);

class Connection : public boost::enable_shared_from_this<Connection>
{
public:
    Connection(net::io_service& ios);
    virtual ~Connection();

    net::ip::tcp::socket& socket();
    void run();

    void set_callback(cb callback);

    request& req();
    void set_req(const std::string& data);
    response& resp();
    void set_resp(const std::string& data);

    void handle_request();

    typedef boost::shared_ptr<Connection> Ptr;
    
private:
    net::ip::tcp::socket socket_;
    boost::array<uint8_t, 8000> data_;

    cb cb_;

    request req_;
    response resp_;
};

#endif // __LYNXHTTP_CONNECTION__