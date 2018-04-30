#include "simulator.h"
#include "components.h"
#include "embed.h"

#include <set>
#include <map>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/program_options.hpp>


using namespace std;

using namespace boost::program_options;

namespace pt = boost::property_tree;

// -- Turn on debugging
bool debug = false;

struct PointData {
  int    size;
  map<string, int> typecount;
};

PointData AggregateData(const std::vector<DPNode<PointData>*>& nodes) {
  int sum = 0;
  map<string, int> typecount;
  for (auto n : nodes) {
    DPLayer<PointData>* layer;
    DPLeaf<PointData>* leaf;
    if ((layer = dynamic_cast<DPLayer<PointData>*>(n))) {
      sum += layer->data.size;

      for (auto pair : layer->data.typecount) {
        if (typecount.count(pair.first)) {
          typecount[pair.first] += pair.second;
        } else {
          typecount[pair.first] = pair.second;
        }
      }

    }

    else if ((leaf = dynamic_cast<DPLeaf<PointData>*>(n))) {
      sum += leaf->data.size;
      for (auto pair : leaf->data.typecount) {
        if (typecount.count(pair.first)) {
          typecount[pair.first] += pair.second;
        } else {
          typecount[pair.first] = pair.second;
        }
      }
    }
  }

  return { sum, move(typecount) };
}

pt::ptree writePT(PointData d) {
    pt::ptree pt;
    pt.put("size", std::to_string(d.size));

    pt::ptree types;
    for (auto pair : d.typecount) {
      types.put(pair.first, pair.second);
    }

    pt.add_child("typecount", types);
    return pt;
}


struct Flags {
  int trimdegree;
};

Flags readFlags(int argc, char *argv[]) {
  Flags flags;
  try {
    options_description desc{"Options"};
    desc.add_options()
      ("help", "Help screen")
      ("trimdegree",  value<int>()->default_value(10), "trim degree");


      variables_map vm;
      store(parse_command_line(argc, argv, desc), vm);
      notify(vm);

      flags.trimdegree = vm["trimdegree"].as<int>();

  } catch (const error &ex) {
    std::cerr << ex.what() << '\n';
    exit(1);
  }
  return flags;
}



int main(int argc, char *argv[]) {
  Flags flags = readFlags(argc, argv);


  cout << "Read names file..." << endl;

  cout << "Start trace..." << endl;
  FILE *f = fdopen(0, "r");
  auto pair = read_trace_file(argv[1], f);
  HeapState Heap = pair.first;
  ExecState Exec = pair.second;
  cout << "DONE" << endl;
  cout << "Done at time " << Exec.Now() << endl;
  
  Graph gTrim = makeGraph(Heap);
  cout << "BEFORE TRIM GRAPH #: " << gTrim.nodes.size() << endl;
  
  trimGraph(&gTrim, flags.trimdegree);
  cout << "AFTER TRIM GRAPH #: " << gTrim.nodes.size() << endl;


  auto components = allComponents(&gTrim);
  cout << "COMPONENT #: " << components.size() << endl;

  std::sort (components.begin(), components.end(), 
            [](const Component& c1, const Component& c2) {
    return c1.nodes.size() > c2.nodes.size();
  });

  for (int i = 0; i < 20; i++) {
    cout << "Component " << i << " : " << components[i].nodes.size() << endl;
  }

  assert(components.size() > 0);
  cout << "USING LARGEST COMPONENT of size" << components[0].nodes.size() << endl;
  int size = components[0].nodes.size();


  cout << "BUILDING DISTANCE MATRIX " << endl;
  auto distance_pairs = build_dist_matrix(components[0]);
  auto distances = distance_pairs.first;
  auto point_mapping = distance_pairs.second;


  cout << "EMBEDDING" << endl;
  auto embedding = embed_graph(distances, size);

  embedding = shift_embedding(embedding, 0, 0);


  cout << "CREATING POINTS " << endl;
  auto points = asPoints(embedding);

  // int i = 0;
  // for (auto p : points) {
  //   cout << i++ << " : " << to_string(p) << endl;
  // }


  int xMax = 0, yMax = 0;
  for (auto p : points) {
    yMax = max(yMax, p.y);
    xMax = max(xMax, p.x);

  }

  cout << "yMax : " << yMax << "  xMax : " << xMax << endl;


  int point_index = 0;
  map<Point, PointData> pointCount;
  for (auto p : points) {
    int id = point_mapping[point_index];
    point_index++;
    string type = Heap.get(id)->getType();

    if (not pointCount.count(p))
      // pointCount[p] = 0;
      pointCount[p] = {0, {{type, 1}}};
    else {
      // pointCount[p]++;
      pointCount[p].size++;

      if (pointCount[p].typecount.count(type)) {
        pointCount[p].typecount[type]++;
      } else {
        pointCount[p].typecount[type] = 1;
      }
      
    }
  }

  cout << "Unique points " << pointCount.size();

  for (auto pair : pointCount) {
    cout << to_string(pair.first) << " : " << pair.second.size << endl; 
  }


  vector<DPLeaf<PointData>*> leaves;
  for (auto pair : pointCount) {
    leaves.push_back( new DPLeaf<PointData>(pair.first, pair.second) );
  }


    Point initial =  Point(0, 0);
    Point distance = Point(xMax, yMax);
    Range scale    = Range(10000, 10000);


  auto root = partitionBy( leaves
                         , initial
                         , distance
                         , scale
                         , AggregateData);

  ofstream outfile;
  outfile.open(argv[2]);

  auto ptree = ptreeOf(*root, writePT);

  write_json(outfile, ptree);

  // vector<int> repeatedPoints;
  // // Point repeat(-8,0);  
  // Point repeat(729,182);
  // for (int i = 0; i < size; i++) {
  //   if (repeat == points[i])
  //     repeatedPoints.push_back(i);
  // }

  // cout << "repeated points # : " << repeatedPoints.size() << endl;

  // for (auto p1 : repeatedPoints) {
  //   cout << to_string(p1) << " ";
  //   for (auto p2 : repeatedPoints) {
  //     cout << " " << distances(p1, p2) << " ";
  //   }
  //   cout << endl;
  // }

}
