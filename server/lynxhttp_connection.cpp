#include <iostream>
#include<boost/make_shared.hpp>

#include "lynxhttp_connection.hpp"

namespace lynxhttp {
namespace server {

connection::connection(net::ip::tcp::socket socket) {
    ssl_enabled_ = false;
    socket_ = boost::make_shared<net::ip::tcp::socket>(std::move(socket));
    req_ = boost::shared_ptr<request>(new request());
}

connection::connection(net::ip::tcp::socket socket, net::ssl::context& ssl_context) {
    ssl_enabled_ = true;
    ssl_socket_ = boost::make_shared<net::ssl::stream<net::ip::tcp::socket> >(std::move(socket), ssl_context);
    req_ = boost::shared_ptr<request>(new request());
}

connection::~connection() {
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
                sp->handle_read();
            }
        });

    } else {
        handle_read();
    }
}

void connection::handle_read() {
    if (ssl_enabled_) {
        ssl_socket_->async_read_some(boost::asio::buffer(data_), 
            [sp = shared_from_this()](const boost::system::error_code& err,
            std::size_t bytes_transferred){
                
                // std::cout << "async_read_some callback" << bytes_transferred << ":" << err << std::endl;
                /*TODO: avoid memory copy.*/
                sp->req().append_data(std::string(sp->data_.begin(), sp->data_.begin() + bytes_transferred));
                
                if (sp->req().parse() == request::parsing_state_t::COMPLETE) {
                    // std::cout << "calling handle request" << std::endl;
                    sp->handle_request();
                } else {
                    sp->handle_read();
                }
            }
        );
    } else {
        socket_->async_read_some(boost::asio::buffer(data_), 
            [sp = shared_from_this()](const boost::system::error_code& err,
            std::size_t bytes_transferred){
                
                // std::cout << "async_read_some callback" << bytes_transferred << ":" << err << std::endl;
                /*TODO: avoid memory copy.*/
                sp->req().append_data(std::string(sp->data_.begin(), sp->data_.begin() + bytes_transferred));
                
                if (sp->req().parse() == request::parsing_state_t::COMPLETE) {
                    // std::cout << "calling handle request" << std::endl;
                    sp->handle_request();
                } else {
                    sp->handle_read();
                }
            }
        );
    }
}

void connection::set_path_tree(path_tree::ptr path_tree) {
    path_tree_ = path_tree;
}

request& connection::req() {
    return *req_;
}

void connection::set_req(const std::string& data) {
    req_->set_data(data);
}

void connection::handle_request() {
    auto resp = boost::shared_ptr<response>(new response());
    resp->set_connection(shared_from_this());
    // std::cout << "calling callback" << std::endl;

    auto header = req_->header();
    auto path = header["path"];

    path_tree_->path_node(path)->get_callback()(req_, resp);
}

} // namespace server
} // namespace lynxhttp