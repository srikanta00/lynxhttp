#ifndef __LYNXHTTP_MESSAGE__
#define __LYNXHTTP_MESSAGE__

#include <string>

#include <boost/asio.hpp>

class Connection;

class request {
public:
    request();
    virtual ~request();
    std::string data() const;
    void set_data(const std::string& data);

private:
    std::string data_;
};

class response {
public:
    response();
    virtual ~response();
    std::string data() const;
    void set_data(const std::string& data);

    void set_connection(const boost::shared_ptr<Connection>& conn);

    void send(const std::string& resp) const;

private:
    std::string data_;
    boost::shared_ptr<Connection> conn_;
};

#endif // __LYNXHTTP_MESSAGE__