#ifndef jsontree_h
#define jsontree_h

#include <functional>
#include <vector>
#include <boost/property_tree/ptree.hpp>


template <class D>
class JSONNode {

public:
    JSONNode(const D d) : data(d), children() { }

    const D& getData() const {
        return data;
    }

    void addChild(JSONNode<D>* child) {
        children.push_back(child);
    }
    void addChildren(std::vector<D*> _children) {
        for (auto c : _children)
            children.push_back(c);
    }

    const std::vector<JSONNode<D>*>& getChildren() const {
        return children;
    }

private:
    const D data;
    std::vector<JSONNode<D>*> children;
};

template <class D>
JSONNode<D>* Node(const D d) {
    return new JSONNode<D>(d);
}

template <class D>
using PTreeFunc = std::function<boost::property_tree::ptree(D)>;

template <class D>
boost::property_tree::ptree ptreeOf(const JSONNode<D>& t,
                                    PTreeFunc<D> writeData,
                                    std::string childrenStr) {
    using boost::property_tree::ptree;
    using std::vector;

    ptree children;
    for (const auto *c : t.getChildren()) {
        children.push_back(std::make_pair("", ptreeOf<D>(*c, writeData)));
    }
    ptree node = writeData(t.getData());
    node.add_child(childrenStr, children);

    return node;
}

#endif
