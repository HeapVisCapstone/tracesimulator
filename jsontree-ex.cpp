#include <iostream>
#include <string>
#include "jsontree.h"


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// Short alias for this namespace
namespace pt = boost::property_tree;


struct Data
{
    int count;
    int sum;    
};

pt::ptree ptreeOfData(Data d) {
    pt::ptree p;
    p.put("count", d.count);
    p.put("sum",   d.sum);
    return p;
}

int main() {
    std::cout << "running jsontree ex" << std::endl;

    auto root = Node<Data>({1, 4});
    root->addChild(Node<Data>({5, 2}));
    root->addChild(Node<Data>({10, 4}));


    auto tree = ptreeOf<Data>(*root, ptreeOfData);

    pt::write_json(std::cout, tree);
}