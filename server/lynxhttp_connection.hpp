#include<string>

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace net = boost::asio;

class Connection : public boost::enable_shared_from_this<Connection>
{
public:
    Connection(net::io_service& ios);
    virtual ~Connection();

    net::ip::tcp::socket& socket();
    void run();

private:

    net::ip::tcp::socket socket_;
    boost::array<uint8_t, 8000> data_;
};