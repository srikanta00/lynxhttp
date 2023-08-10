#include <iostream>

#include "lynxhttp_path_tree.hpp"

#include <boost/algorithm/string.hpp>

path_tree_node::path_tree_node(const std::string& key) {
    key_ = key;
}

path_tree_node::path_tree_node(const std::string& key, callback cb) {
    key_ = key;
    cb_ = cb;
}

path_tree_node::~path_tree_node() {

}

path_tree_node::ptr path_tree_node::add_child(std::string& key, callback cb) {
    auto child = add_child(key);
    child->set_callback(cb);

    return child;
}

path_tree_node::ptr path_tree_node::add_child(std::string& key) {
    auto child = std::make_shared<path_tree_node>(key);
    child_[key] = child;

    return child;
}

void path_tree_node::set_callback(callback cb) {
    cb_ = cb;
}
callback path_tree_node::get_callback() {
    return cb_;
}

path_tree_node::ptr path_tree_node::get_child(std::string& key) {
    if(child_.find(key) == child_.end()) return std::shared_ptr<path_tree_node>(nullptr);
    return child_[key];
}

///////////////////////////////////////////////////////////////////////////
path_tree::path_tree() {

}

path_tree::~path_tree() {

}

void path_tree::add_path(const std::string& path, callback cb) {
    auto p = path.find("/");
    if (p == std::string::npos) return;

    std::vector<std::string> strs;
    boost::split(strs, path, boost::is_any_of("/"));

    if(head_ == nullptr) {
        head_ = std::make_shared<path_tree_node>("root");
    }

    auto node = head_;

    for (auto str : strs) {
        if (str.length() == 0) continue;

        node = node->add_child(str);
    }

    node->set_callback(cb);
}

path_tree_node::ptr path_tree::path_node(std::string& path) {
    auto node = head_;

    std::vector<std::string> strs;
    boost::split(strs, path, boost::is_any_of("/"));

    for (auto str : strs) {
        if (str.length() == 0) continue;

        auto n = node->get_child(str);
        if (!n) break;

        node = n;
    }

    return node;
}