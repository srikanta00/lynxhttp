#ifndef __LYNXHTTP_MESSAGE__
#define __LYNXHTTP_MESSAGE__

#include <string>
#include <map>

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
    int parse();
    const std::map<std::string, std::string> header() const;
    const std::string& body() const;

    typedef boost::shared_ptr<request> ptr;

private:
    std::map<std::string, std::string> header_;
    std::string body_;
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

    void send(const std::string& resp);
    void send(int status_code, const std::string& resp);

    typedef boost::shared_ptr<response> ptr;

    std::string serialize();

private:
    int status_code_;
    std::map<std::string, std::string> header_;
    std::string body_;

    std::string data_;
    boost::shared_ptr<connection> conn_;
};

#endif // __LYNXHTTP_MESSAGE__