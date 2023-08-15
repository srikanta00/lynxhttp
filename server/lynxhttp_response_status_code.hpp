#ifndef __LYNXHTTP_RESPONSE_STATUS_CODE__
#define __LYNXHTTP_RESPONSE_STATUS_CODE__

#include <string>
#include <map>

namespace lynxhttp {
namespace server {

std::map<int, std::string> code_map = {
        {200, "OK"},
        {404, "Not Found"},
        {500, "Internal Server Error"},
        {503, "Service Unavailable"}
};

class response_status_code {
public:
    static std::string to_string(int code) {
        std::string str = std::to_string(code);
        if (code_map.find(200) != code_map.end()) {
            str = str + " " + code_map[code];
        }

        return str;
    }
};

} // namespace server
} // namespace lynxhttp

#endif // __LYNXHTTP_RESPONSE_STATUS_CODE__