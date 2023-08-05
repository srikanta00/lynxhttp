#ifndef __LYNXHTTP_MESSAGE__
#define __LYNXHTTP_MESSAGE__

#include <string>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

class connection;

class request : public boost::enable_shared_from_this<request>
{
public:
    request();
    virtual ~request();
    std::string data() const;
    void set_data(const std::string& data);

    typedef boost::shared_ptr<request> ptr;

private:
    std::string data_;
};

class response : public boost::enable_shared_from_this<response>
{
public:
    response();
    virtual ~response();
    std::string data() const;
    void set_data(const std::string& data);

    void set_connection(const boost::shared_ptr<connection>& conn);

    void send(const std::string& resp) const;

    typedef boost::shared_ptr<response> ptr;

private:
    std::string data_;
    boost::shared_ptr<connection> conn_;
};

#endif // __LYNXHTTP_MESSAGE__