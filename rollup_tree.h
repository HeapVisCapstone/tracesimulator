#ifndef aggtree_h
#define aggtree_h

#include <string>
#include <map> 
#include <vector> 
#include <assert.h>
#include <functional>
#include <boost/property_tree/ptree.hpp>
#include <iostream>


template <typename D>
class RollupNode
{
public:
// TODO: add aggregate from nodes functionality
    RollupNode(const std::string& n, D d) : 
        id(n), data(d) { }

    RollupNode(D d) : 
        id(""), data(d) { }

    void addChild(RollupNode<D>* node) {    
        children.push_back(node);
    }

    std::string getId() const { return id; }
    D getData() const { return data; }
    const std::vector<RollupNode<D>*>& getChildren() const 
            { return children; } 

private:
    std::string id;
    std::vector<RollupNode<D>*> children;
    D data;

};


// Small DSL for building RollupNode
template <typename D>
RollupNode<D>* node(const std::string& s, D d) {
    auto n = new RollupNode<D>(s, d);    
    // n->addChild(e);
    return n;
}

template <typename D, typename... Args>
RollupNode<D>* node(const std::string& s, D d, RollupNode<D>* tree,  Args... args) {
    auto n = node(s, d, args...);
    n->addChild(tree);
    return n;
}

// Terminal constructor
template <typename D>
RollupNode<D>* leaf(const D& d) {
    return new RollupNode<D>(d);
}



template <class D>
using Encoding = std::function<boost::property_tree::ptree(D)>;


// string pointKey()

template <typename D>
boost::property_tree::ptree ptreeOf(const RollupNode<D>& node,
                                    Encoding<D> writeData) {
    using std::vector;

    boost::property_tree::ptree pt;
    pt.put("id",     node.getId());
    pt.add_child("data", writeData(node.getData()));

    boost::property_tree::ptree children;
    for (auto child : node.getChildren()) {
        children.push_back(std::make_pair("", ptreeOf(*child, writeData)));
    }
    if (not children.empty())
        pt.add_child("children", children);



    // ptree node = writeData(t.getData());


    // ptree children;
    // for (const auto *c : t.getChildren()) {
    //     children.push_back(std::make_pair("", ptreeOf<D>(*c, writeData)));
    // }
    // ptree node = writeData(t.getData());
    // node.add_child(childrenStr, children);

    return pt;
}


#endif // aggtree_h