#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "lynxhttp_client.hpp"

namespace net = boost::asio;

namespace lynxhttp {
namespace client {

class client::Impl {
public:
    Impl(std::string host, unsigned short port, int n_threads, bool ssl_enabled) :
    host_(host), port_(port), n_threads_(n_threads), ssl_enabled_(ssl_enabled_),
    stopped_(false), socket_(ios_), deadline_(ios_), resolver_(ios_) {
        connection_cb_ = nullptr;

    }

    virtual ~Impl() {
    }

    void run();
    void stop();

    void on_connect(connection_cb cb);

private:
    net::io_service ios_;
    int n_threads_;
    boost::thread_group thread_group_;

    std::string host_;
    unsigned short port_;
    bool ssl_enabled_;

    bool stopped_;
    net::ip::tcp::socket socket_;
    net::streambuf stream_buff_;
    net::deadline_timer deadline_;
    net::ip::tcp::resolver resolver_;
    connection_cb connection_cb_;

    void start_connect(net::ip::tcp::resolver::iterator ep_iter);
    void check_deadline();
    void handle_connect(const boost::system::error_code& ec,
      net::ip::tcp::resolver::iterator endpoint_iter);
};

client::client(std::string host, unsigned short port, int n_threads, bool ssl_enabled) {
    impl_ = new Impl(host, port, n_threads, ssl_enabled);
}

client::~client() {
    if (impl_) delete impl_;
}

void client::run() {
    impl_->run();
}

void client::on_connect(connection_cb cb) {
    impl_->on_connect(cb);
}

void client::stop() {
    impl_->stop();
}

//////////////////////////////////////////////////////////////////////////////////////

void client::Impl::run() {
    start_connect(resolver_.resolve(net::ip::tcp::resolver::query(host_, std::to_string(port_))));

    deadline_.async_wait(std::bind(&client::Impl::check_deadline, this));

    for (auto i = 0; i < n_threads_ - 1; i++) {
        thread_group_.add_thread(new boost::thread([this](){
            ios_.run();
        }));
    }

    ios_.run();

}

void client::Impl::stop() {
    std::cout << "client stop called." << std::endl;
    stopped_ = true;
    socket_.close();
    deadline_.cancel();

    thread_group_.join_all();
    std::cout << "client stopped." << std::endl;
}

void client::Impl::start_connect(net::ip::tcp::resolver::iterator ep_iter) {
    if (ep_iter != net::ip::tcp::resolver::iterator()) {
        std::cout << "Connecting " << ep_iter->endpoint() << "..." << std::endl;

        deadline_.expires_from_now(boost::posix_time::seconds(30));
        
        socket_.async_connect(ep_iter->endpoint(), 
                    std::bind(&client::Impl::handle_connect, this, std::placeholders::_1, ep_iter));
    } else {
        stop();
        boost::system::error_code ec = boost::system::errc::make_error_code(boost::system::errc::timed_out);
        if (connection_cb_) connection_cb_(ec);
    }
}

void client::Impl::handle_connect(const boost::system::error_code& ec,
      net::ip::tcp::resolver::iterator ep_iter) {

    if (stopped_) return;

    if (!socket_.is_open()) {
        std::cout << "Connection timed out." << std::endl;
        
        start_connect(++ep_iter);
    } else {
        std::cout << "Connected to " << ep_iter->endpoint() << std::endl;
        if (connection_cb_) connection_cb_(ec);
    }
}

void client::Impl::check_deadline() {
    if (stopped_) return;

    if (deadline_.expires_at() <= net::deadline_timer::traits_type::now()) {
        socket_.close();
        deadline_.expires_at(boost::posix_time::pos_infin);
    }

    deadline_.async_wait(std::bind(&client::Impl::check_deadline, this));
}

void client::Impl::on_connect(connection_cb cb) {
    connection_cb_ = cb;
}

} // namespace client
} // namespace lynxhttp