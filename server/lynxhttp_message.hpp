#ifndef __LYNXHTTP_MESSAGE__
#define __LYNXHTTP_MESSAGE__

#include <string>
#include <map>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace lynxhttp {
namespace server {

class connection;

class request : public boost::enable_shared_from_this<request>
{
public:
    request();
    virtual ~request();
    std::string data() const;
    void set_data(const std::string& data);
    void append_data(const std::string& data);

    const std::map<std::string, std::string>& header() const;
    const std::map<std::string, std::string>& query_params() const;
    const std::string& body() const;

    typedef boost::shared_ptr<request> ptr;

    enum parsing_state_t {
        EMPTY,
        INCOMPLETE,
        COMPLETE,
        ERROR
    };

    parsing_state_t parse();
    parsing_state_t parsing_state();

private:
    void parse_query_params(std::string& params);
    
    std::map<std::string, std::string> header_;
    std::map<std::string, std::string> query_params_;
    std::string body_;
    std::string data_;

    parsing_state_t parsing_state_;
    int parsed_len_;
};

class response : public boost::enable_shared_from_this<response>
{
public:
    response();
    virtual ~response();
    std::string data() const;
    void set_data(const std::string& data);

    void set_connection(const boost::shared_ptr<connection>& conn);
    boost::shared_ptr<connection> get_connection();

    void send(const std::string& resp);
    void send(int status_code, const std::string& resp);
    void end(int status_code, const std::string& resp);

    bool conn_continue();

    typedef boost::shared_ptr<response> ptr;

    std::string serialize();

private:
    void start_send(int status_code, const std::string& resp);

    int status_code_;
    std::map<std::string, std::string> header_;
    std::string body_;

    std::string data_;
    boost::shared_ptr<connection> conn_;
    bool conn_continue_;
};

} // namespace server
} // namespace lynxhttp

#endif // __LYNXHTTP_MESSAGE__