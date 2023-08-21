#include <iostream>

#include <boost/asio.hpp>
#include <boost/array.hpp>
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

    request::ptr send(const std::string& method, const std::string& url, const std::string& data);

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

    boost::array<uint8_t, 8000> data_;

    void start_connect(net::ip::tcp::resolver::iterator ep_iter);
    void check_deadline();
    void handle_connect(const boost::system::error_code& ec,
      net::ip::tcp::resolver::iterator endpoint_iter);
    void start_read(request::ptr req, response::ptr resp);
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

request::ptr client::send(const std::string& method, const std::string& url, const std::string& data) {
    return impl_->send(method, url, data);
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

request::ptr client::Impl::send(const std::string& method, const std::string& url, const std::string& data) {
    std::string path = "/";
    int i = url.find("/");

    if (i != url.npos) {
        path = url.substr(i);
    }

    auto req = boost::make_shared<request>(method, path, data);

    socket_.async_write_some(boost::asio::buffer(req->serialize()),
            [this, req](const boost::system::error_code& err,
                            std::size_t bytes_transferred) {
                // std::cout << "Message sent" << std::endl;
                auto resp = boost::make_shared<response>();
                start_read(req, resp);
            }
        );
    return req;
}

void client::Impl::start_read(request::ptr req, response::ptr resp) {
    socket_.async_read_some(boost::asio::buffer(data_), [this, req, resp](const boost::system::error_code& ec,
                            std::size_t bytes_transferred){
        resp->append_data(std::string(data_.begin(), data_.begin() + bytes_transferred));
        
        if (resp->parse() == response::parsing_state_t::COMPLETE) {
            // std::cout << "calling response cb" << std::endl;
            req->get_response_cb()(resp);
        } else {
            // std::cout << "incomplete response parsing" << std::endl;
            start_read(req, resp);
        }

    });
}

} // namespace client
} // namespace lynxhttp