#include <lynxhttp_server.hpp>

server::server() {
    cout << "constructor" << endl;
}

server::~server() {
    cout << "destructor" << endl;
}

void server::identity() {
    cout << "identity" << endl;
}