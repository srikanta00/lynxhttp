#ifndef __LYNXHTTP_SERVER__
#define __LYNXHTTP_SERVER__

#include <iostream>

#include "lynxhttp_message.hpp"
#include "lynxhttp_connection.hpp"
#include "lynxhttp_path_tree.hpp"

namespace lynxhttp {
namespace server {

class server 
{
public:
    server(std::string address = "0.0.0.0", unsigned short port = 80, bool ssl = false, int n_threads = 2);
    virtual ~server();

    void serve();

    void handle(const std::string& path, callback cb);

    void set_certificate_chain_file(std::string cert_file);
    void set_private_key_file(std::string key_file);
    void set_private_key_password(std::string passwd);

private:
    class Impl;
    Impl *impl_ = nullptr;
};

} // namespace server
} // namespace lynxhttp

#endif // __LYNXHTTP_SERVER__