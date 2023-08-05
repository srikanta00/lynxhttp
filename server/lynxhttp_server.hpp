#include <iostream>
using namespace std;

class Server 
{
public:
    Server();
    virtual ~Server();

    void serve();

private:
    class Impl;
    Impl *impl_ = nullptr;
};