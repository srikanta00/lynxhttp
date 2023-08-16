#include "lynxhttp_client.hpp"

namespace lynxhttp {
namespace client {

client::client() {
    a = 10;
}

client::~client() {
    a = 0;
}

} // namespace client
} // namespace lynxhttp