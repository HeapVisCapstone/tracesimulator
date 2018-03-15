#include "rollup_tree.h"


#include <iostream>
#include <string>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// Short alias for this namespace
namespace pt = boost::property_tree;


struct Data {
    int row, col, size, height, width;
};  


pt::ptree writePT(Data d) {
    pt::ptree pt;
    pt.put("row",    std::to_string(d.row));
    pt.put("col",    std::to_string(d.col));
    pt.put("size",   std::to_string(d.size));
    pt.put("height", std::to_string(d.height));
    pt.put("width",  std::to_string(d.width));

    return pt;
}





int main() {
    auto rep = node( "top", Data {0, 0, 10, 100, 100}
                   , leaf(Data {0, 30, 4, 1, 1}), leaf(Data {0, 0, 1, 1, 1}),
                       node( "bottom", Data {0, 0, 10, 100, 100}
                           , leaf(Data {0, 0, 2, 1, 1})
                           , leaf(Data {0, 3, 3, 1, 1})));

    auto tree = ptreeOf<Data>(*rep, writePT);

    pt::write_json(std::cout, tree);
}