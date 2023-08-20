#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/thread.hpp>

#include "lynxhttp_server.hpp"


namespace net = boost::asio;

namespace lynxhttp {
namespace server {

class server::Impl 
{
public:
    Impl(std::string address, unsigned short port, bool ssl, int n_threads) :
    address_(address), port_(port), ssl_enabled_(ssl),
    ssl_context_(net::ssl::context::sslv23), n_threads_(n_threads) {
        stopped_ = false;
        path_tree_ = std::make_shared<path_tree>();
    }

    void run(net::ip::tcp::endpoint& endpoint);
    void serve();
    void stop();

    void add_path(const std::string& path, callback cb);
    void set_certificate_chain_file(std::string cert_file) {
        cert_file_ = cert_file;
    }

    void set_private_key_file(std::string key_file) {
        priv_key_file_ = key_file;
    }

    void set_private_key_password(std::string passwd) {
        priv_key_passwd_ = passwd;
    }

private:
    std::string get_password() {
        return priv_key_passwd_;
    }

    net::io_service ios_;
    std::shared_ptr<net::ip::tcp::acceptor> acceptor_; //{ioc, endpoint};
    
    bool ssl_enabled_;
    net::ssl::context ssl_context_;

    std::string cert_file_;
    std::string priv_key_file_;
    std::string priv_key_passwd_;

    std::string address_;
    unsigned short port_;
    path_tree::ptr path_tree_;

    int n_threads_;
    boost::thread_group thread_group_;

    bool stopped_;
};

server::server(std::string address, unsigned short port, bool ssl, int n_threads) {
    impl_ = new Impl(address, port, ssl, n_threads);
}

server::~server() {
    if (impl_) delete impl_;
}

void server::serve() {
    impl_->serve();
}

void server::stop() {
    impl_->stop();
}

void server::handle(const std::string& path, callback cb) {
    impl_->add_path(path, cb);
}

void server::set_certificate_chain_file(std::string cert_file) {
    impl_->set_certificate_chain_file(cert_file);
}

void server::set_private_key_file(std::string key_file) {
    impl_->set_private_key_file(key_file);
}

void server::set_private_key_password(std::string passwd) {
    impl_->set_private_key_password(passwd);
}

////////////////////////////////////////////////////////////////////////////////////////

void server::Impl::run(net::ip::tcp::endpoint& ep) {
    /*
    auto conn = boost::shared_ptr<connection>(new connection(ios_, ssl_context_));

    conn->set_callback(cb_);

    acceptor_->async_accept(conn->socket(),
                        [this, conn, &ep](const error_code& ec)
                        {
                            cout << "handler called:" << ec << endl;
                            if(!ec)
                            {
                                conn->run();
                            }

                            this->run(ep);
                        });
    */
   acceptor_->async_accept([this, &ep](const std::error_code& ec, net::ip::tcp::socket socket)
                        {
                            if(!ec)
                            {
                                // auto conn = boost::shared_ptr<connection>(new connection(std::move(socket), ssl_context_));
                                
                                boost::shared_ptr<connection> conn;
                                if (ssl_enabled_) {
                                    conn = boost::shared_ptr<connection>(new connection(std::move(socket), ssl_context_));
                                } else {
                                    conn = boost::shared_ptr<connection>(new connection(std::move(socket)));
                                }
                                conn->set_path_tree(path_tree_);
                                conn->run();
                            }

                            if (!stopped_) this->run(ep);
                        });
}

void server::Impl::serve() {
    std::cout << "Server started." << std::endl;

    if (ssl_enabled_) {
        ssl_context_.set_options(
            boost::asio::ssl::context::default_workarounds
            | boost::asio::ssl::context::no_sslv2
            | boost::asio::ssl::context::single_dh_use);

    ssl_context_.set_password_callback(std::bind(&server::Impl::get_password, this));
    ssl_context_.use_certificate_chain_file(cert_file_);
    ssl_context_.use_private_key_file(priv_key_file_, boost::asio::ssl::context::pem);
    // ss_context_.use_tmp_dh_file("dh2048.pem");
    }

    auto address = net::ip::make_address(address_);
    // net::ip::tcp::endpoint endpoint{net::ip::tcp::v4(), port_};
    net::ip::tcp::endpoint endpoint{address, port_};
    acceptor_ = std::make_shared<net::ip::tcp::acceptor>(ios_, endpoint);
    acceptor_->listen();
    run(endpoint);
    
    for (auto i = 0; i < n_threads_ - 1; i++) {
        thread_group_.add_thread(new boost::thread([this](){
            ios_.run();
        }));
    }

    ios_.run();
}

void server::Impl::stop() {
    net::post(acceptor_->get_executor(), [this](){
        stopped_ = true;
        acceptor_->cancel();
    });

    thread_group_.join_all();
}

void server::Impl::add_path(const std::string& path, callback cb) {
    path_tree_->add_path(path, cb);
}

} // namespace server
} // namespace lynxhttp