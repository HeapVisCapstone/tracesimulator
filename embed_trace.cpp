#include "simulator.h"
#include "components.h"
#include "embed.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


using namespace std;


namespace pt = boost::property_tree;

// -- Turn on debugging
bool debug = false;

int AggregateData(const std::vector<DPNode<int>*>& nodes) {
  int sum = 0;
  for (auto n : nodes) {
    DPLayer<int>* layer;
    DPLeaf<int>* leaf;
    if ((layer = dynamic_cast<DPLayer<int>*>(n)))
      sum += layer->data;
    else if ((leaf = dynamic_cast<DPLeaf<int>*>(n)))
      sum += leaf->data;
  }

  return sum;
}

pt::ptree writePT(int d) {
    pt::ptree pt;
    pt.put("size",   std::to_string(d));

    return pt;
}


void analyze();

int main(int argc, char *argv[]) {
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
  
  trimGraph(&gTrim, 3);
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
  auto distances = build_dist_matrix(components[0]);

  for (int i = 0; i < size; i++) {
    int sum = 0;
    // cout << "NODE " << i;
    for (int j = 0; j < size; j++) {
      sum += distances(i, j);
      // cout << " " << distances(i, j) << " ";
    }
    // cout << endl;
    cout << "NODE " << i << " AVERAGE DISTANCE TO EVERYTHING ELSE " << (float)sum/(float)size << endl;

  }

  cout << "EMBEDDING" << endl;
  auto embedding = embed_graph(distances, size);

  embedding = shift_embedding(embedding, 0, 0);


  cout << "CREATING POINTS " << endl;
  auto points = asPoints(embedding);

  int i = 0;
  for (auto p : points) {
    cout << i++ << " : " << to_string(p) << endl;
  }


  int xMax = 0, yMax = 0;
  for (auto p : points) {
    yMax = max(yMax, p.y);
    xMax = max(xMax, p.x);

  }

  cout << "yMax : " << yMax << "  xMax : " << xMax << endl;


  map<Point, int> pointCount;
  for (auto p : points) {
    if (not pointCount.count(p))
      pointCount[p] = 0;
    pointCount[p]++;
  }

  cout << "Unique points " << pointCount.size();

  for (auto pair : pointCount) {
    cout << to_string(pair.first) << " : " << pair.second << endl; 
  }


  vector<DPLeaf<int>*> leaves;
  for (auto pair : pointCount) {
    leaves.push_back( new DPLeaf<int>(pair.first, pair.second) );
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
