#ifndef __LYNXHTTP_CLIENT__
#define __LYNXHTTP_CLIENT__

#include <string>

namespace lynxhttp {
namespace client {

class client {
public:
    client(std::string host = "localhost", unsigned short port = 80, int n_threads = 2, bool ssl_enabled = false);
    virtual ~client();

private:
    class Impl;
    Impl* impl_;
    int a;
};

} // namespace client
} // namespace lynxhttp

#endif // __LYNXHTTP_CLIENT__