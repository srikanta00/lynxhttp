#include <iostream>
#include<boost/make_shared.hpp>

#include "lynxhttp_connection.hpp"

namespace lynxhttp {
namespace server {

connection::connection(boost::shared_ptr<net::ip::tcp::socket> socket) {
    ssl_enabled_ = false;
    socket_ = socket;
    // req_ = boost::shared_ptr<request>(new request());
}

connection::connection(boost::shared_ptr<net::ssl::stream<net::ip::tcp::socket> > socket, net::ssl::context& ssl_context) {
    ssl_enabled_ = true;
    ssl_socket_ = socket;
    // req_ = boost::shared_ptr<request>(new request());
}

connection::~connection() {
    if (ssl_enabled_) {
        ssl_socket_->shutdown();
        socket_pool_->remove(ssl_socket_);
    } else {
        socket_->close();
        socket_pool_->remove(socket_);
    }

    // std::cout << "connection::destructor called" << std::endl;
}

net::ssl::stream<net::ip::tcp::socket>& connection::ssl_socket() {
    return *ssl_socket_;
}

bool connection::ssl_enabled() {
    return ssl_enabled_;
}

net::ip::tcp::socket& connection::socket() {
    return *socket_;
}

void connection::run() {
    // std::cout << "connection::run" << std::endl;
    
    if(ssl_enabled_) {
        
        ssl_socket_->async_handshake(boost::asio::ssl::stream_base::server, 
        [sp = shared_from_this()](const boost::system::error_code& error)
        {
            // std::cout << "async_handshake callback" << std::endl;
            if (!error)
            {
                // std::cout << "calling sp->handle_read" << std::endl;
                sp->start_read();
            }
        });

    } else {
        start_read();
    }
}

void connection::start_read() {
    auto req = boost::make_shared<request>();

    handle_read(req);
}

void connection::handle_read(request::ptr req) {
    if (ssl_enabled_) {
        ssl_socket_->async_read_some(boost::asio::buffer(data_), 
            [sp = shared_from_this(), req](const boost::system::error_code& ec,
            std::size_t bytes_transferred){
                if (ec) return;
                // std::cout << "async_read_some callback" << bytes_transferred << ":" << err << std::endl;
                /*TODO: avoid memory copy.*/
                req->append_data(std::string(sp->data_.begin(), sp->data_.begin() + bytes_transferred));
                
                if (req->parse() == request::parsing_state_t::COMPLETE) {
                    // std::cout << "calling handle request" << std::endl;
                    sp->handle_request(req);
                } else {
                    sp->handle_read(req);
                }
            }
        );
    } else {
        socket_->async_read_some(boost::asio::buffer(data_), 
            [sp = shared_from_this(), req](const boost::system::error_code& ec,
            std::size_t bytes_transferred){
                if (ec) return;
                // std::cout << "async_read_some callback" << bytes_transferred << ":" << err << std::endl;
                /*TODO: avoid memory copy.*/
                req->append_data(std::string(sp->data_.begin(), sp->data_.begin() + bytes_transferred));
                
                if (req->parse() == request::parsing_state_t::COMPLETE) {
                    // std::cout << "calling handle request" << std::endl;
                    sp->handle_request(req);
                } else {
                    sp->handle_read(req);
                }
            }
        );
    }
}

void connection::set_path_tree(path_tree::ptr path_tree) {
    path_tree_ = path_tree;
}

void connection::set_socket_pool(socket_pool::ptr socket_pool) {
    socket_pool_ = socket_pool;
}
/*
request& connection::req() {
    return *req_;
}


void connection::set_req(const std::string& data) {
    req_->set_data(data);
}
*/

void connection::handle_request(request::ptr req) {
    auto resp = boost::shared_ptr<response>(new response());
    resp->set_connection(shared_from_this());
    resp->set_req(req);
    // std::cout << "calling callback" << std::endl;

    auto header = req->header();
    auto path = header["path"];

    path_tree_->path_node(path)->get_callback()(req, resp);
}


////////////////////////////////////////////////////////////////////////////////////

void socket_pool::add(boost::shared_ptr<net::ip::tcp::socket> socket) {
    sockets_.push_back(socket);
}

void socket_pool::add(boost::shared_ptr<net::ssl::stream<net::ip::tcp::socket>> socket) {
    ssl_sockets_.push_back(socket);
}

void socket_pool::remove(boost::shared_ptr<net::ip::tcp::socket> socket) {
    auto it = std::find(sockets_.begin(), sockets_.end(), socket);
    if (it != sockets_.end()) sockets_.erase(it);
}

void socket_pool::remove(boost::shared_ptr<net::ssl::stream<net::ip::tcp::socket>> socket) {
    auto it = std::find(ssl_sockets_.begin(), ssl_sockets_.end(), socket);
    if (it != ssl_sockets_.end())  ssl_sockets_.erase(it);
}

void socket_pool::close() {
    
    for (auto s : sockets_) s->cancel();
    sockets_.clear();

    for (auto s : ssl_sockets_) s->shutdown();
    ssl_sockets_.clear();
}

} // namespace server
} // namespace lynxhttp