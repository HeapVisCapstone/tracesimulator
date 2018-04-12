#include "rollup_tree.h"
#include "planar_tree_discrete.h"


#include <iostream>
#include <string>
#include <map>
#include <cstdlib>
#include <functional>

#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

// Short alias for this namespace
namespace pt = boost::property_tree;
using namespace boost::program_options;



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
    if ((layer = dynamic_cast<DPLayer<Data>*>(n)))
      sum += layer->data.size;
    else if ((leaf = dynamic_cast<DPLeaf<Data>*>(n)))
      sum += leaf->data.size;
  }

  return Data {sum};
}

int main(int argc, const char *argv[]) { 
  Point initial(0, 0);
  Point distance(100, 100);
  Range scale;;
  Range rRange;
  Range cRange;

  Range sizeRange;
  int count;


  try {
    options_description desc{"Options"};
    desc.add_options()
      ("help", "Help screen")
      ("height",  value<int>()->default_value(0), "Height")
      ("width",   value<int>()->default_value(0), "Width")
      ("rscale",  value<int>()->default_value(0), "Row Scale")
      ("cscale",  value<int>()->default_value(0), "Col Scale")
      ("sizemin", value<int>()->default_value(0), "Min leaf size")
      ("sizemax", value<int>()->default_value(0), "Max leaf size")
      ("count",  value<int>()->default_value(0), "Number of target leaves");

      variables_map vm;
      store(parse_command_line(argc, argv, desc), vm);
      notify(vm);


      initial =  Point(0, 0);
      distance = Point(vm["height"].as<int>(), vm["width"].as<int>());
      scale    = Range(vm["rscale"].as<int>(), vm["cscale"].as<int>());

      rRange = range(0, vm["height"].as<int>());
      cRange = range(0, vm["width"].as<int>());

      sizeRange = range(vm["sizemin"].as<int>(), vm["sizemax"].as<int>());

      count = vm["count"].as<int>();

  } catch (const error &ex) {
    std::cerr << ex.what() << '\n';
  }



  auto points = gen( sizeRange   // size
                   , rRange // rows
                   , cRange // cols
                   , count);

  auto root = partitionBy( points
                         , initial
                         , distance
                         , scale
                         , AggregateData);

  assert(root);

  auto ptree = ptreeOf(*root, writePT);

  pt::write_json(std::cout, ptree);
}