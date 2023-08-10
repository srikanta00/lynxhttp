#ifndef __LYNXHTTP_PATH_TREE__
#define __LYNXHTTP_PATH_TREE__

#include <string>
#include <vector>

#include "lynxhttp_message.hpp"

typedef void(*callback)(const request::ptr req, const response::ptr resp);

class path_tree_node {
public:
    path_tree_node(const std::string& key, callback cb);
    path_tree_node(const std::string& key);

    virtual ~path_tree_node();
    typedef std::shared_ptr<path_tree_node> ptr;

    ptr add_child(std::string& key, callback cb);
    ptr add_child(std::string& key);
    void set_callback(callback cb);
    callback get_callback();

    ptr get_child(std::string& key);

    std::string key_;
private:
    
    callback cb_;
    std::map<std::string, ptr> child_;
};

class path_tree {
public:
    path_tree();
    virtual ~path_tree();

    void add_path(const std::string& path, callback cb);
    path_tree_node::ptr path_node(std::string& path);

    typedef std::shared_ptr<path_tree> ptr;

private:
    path_tree_node::ptr head_;
};

#endif // __LYNXHTTP_PATH_TREE__