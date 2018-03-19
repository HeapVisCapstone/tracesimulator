#include "rollup_tree.h"
#include "planar_tree_discrete.h"


#include <iostream>
#include <string>
#include <map>
#include <cstdlib>
#include <functional>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// Short alias for this namespace
namespace pt = boost::property_tree;



struct Data {
    int size;
};  


pt::ptree writePT(Data d) {
    pt::ptree pt;
    pt.put("size",   std::to_string(d.size));

    return pt;
}


int randInRange(Range r) {
  int x1 = r.first;
  int x2 = r.second;
  int xd = x2 - x1;
  return (rand() % xd) + x1; 
}

// Tries to make n random points. Drops overlaps.
std::vector<DPLeaf<Data>*> gen(Range sizeR, Range hR, Range wR, int n) {
    std::set<Point> points;
    std::vector<DPLeaf<Data>*> leaves;

    for (int i = 0; i < n; i++) {
      Point p(randInRange(hR), randInRange(wR));
      Data  d = Data {randInRange(sizeR)};
      // std::cerr << "Considering point: " << to_string(p) << std::endl;
      
      if (points.insert(p).second) {
        // std::cerr << "Adding point: " << to_string(p) << std::endl;
        auto leaf = new DPLeaf<Data>(p, d);
        // leaf->p0 = p;
        // leaf->data = randInRange(sizeR);
        leaves.push_back(leaf);
      }
    }
    return std::move(leaves);
}



Data AggregateData(const std::vector<DPNode<Data>*>& nodes) {
  int sum = 0;
  for (auto n : nodes) {
    DPLayer<Data>* layer;
    DPLeaf<Data>* leaf;
    if (layer = dynamic_cast<DPLayer<Data>*>(n))
      sum += layer->data.size;
    else if (leaf = dynamic_cast<DPLeaf<Data>*>(n))
      sum += leaf->data.size;
  }

  return Data {sum};
}

int main() {                   
    auto points = gen( range(0, 10)   // size
                     , range(0, 10000) // rows
                     , range(0, 10000) // cols
                     , 100000);

    auto root = partitionBy( points
                           , Point(0, 0)
                           , Point(10000, 10000)
                           , range(50, 50)
                           , AggregateData);

    assert(root);

    auto ptree = ptreeOf(*root, writePT);

    pt::write_json(std::cout, ptree);
}