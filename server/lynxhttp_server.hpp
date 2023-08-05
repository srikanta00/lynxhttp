#ifndef __LYNXHTTP_SERVER__
#define __LYNXHTTP_SERVER__

#include <iostream>

#include "lynxhttp_message.hpp"
#include "lynxhttp_connection.hpp"

using namespace std;

class Server 
{
public:
    Server();
    virtual ~Server();

    void serve();

    void handle(const std::string& path, cb callback);

private:
    cb cb_;

    class Impl;
    Impl *impl_ = nullptr;
};

#endif // __LYNXHTTP_SERVER__