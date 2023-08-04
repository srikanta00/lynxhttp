#include <iostream>
#include <boost/asio.hpp>

using namespace std;

namespace net = boost::asio;

class server 
{
public:
    server();
    virtual ~server();
    void identity();

    void run(net::ip::tcp::endpoint&);
    void serve();

private:
    net::io_service ioc;
    
    shared_ptr<net::ip::tcp::acceptor> acceptor; //{ioc, endpoint};

    int tmp = 0;
};