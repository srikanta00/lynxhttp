#include <lynxhttp_server.hpp>

int tmp = 0;

void accept_and_run(net::ip::tcp::acceptor& acceptor, net::io_service& io_service, net::ip::tcp::endpoint& ep)
{
   // std::shared_ptr<session> sesh = std::make_shared<session>(io_service);
   
   auto socket = make_shared<net::ip::tcp::socket>(io_service, ep.protocol());

   acceptor.async_accept(*socket,
   [&acceptor, &io_service, &ep](const error_code& accept_error)
   {
      cout << "handler called:" << accept_error << ":" << tmp++ << endl;
      accept_and_run(acceptor, io_service, ep);

      if(!accept_error)
      {
         cout << "data received." << endl;
      }
   });
}


int main() {
/*
    server srv;

    srv.identity();

    srv.serve();
*/
    net::io_service io_service;

    auto const address = net::ip::make_address("192.168.222.130");

    net::ip::tcp::endpoint endpoint{address, 4224};
    net::ip::tcp::acceptor acceptor{io_service};

    acceptor.open(endpoint.protocol());
    acceptor.set_option(net::socket_base::reuse_address(true));
    acceptor.bind(endpoint);
    
    acceptor.listen();
    accept_and_run(acceptor, io_service, endpoint);
    
    io_service.run();

    return 0;
}