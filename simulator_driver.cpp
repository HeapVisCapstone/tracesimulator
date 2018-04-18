#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <vector>
#include <algorithm>

namespace pt = boost::property_tree;
using namespace std;

#include "classinfo.h"
#include "execution.h"
#include "heap.h"
#include "tokenizer.h"

#include "simulator.h"
#include "components.h"

class Object;
class CCNode;

// ----------------------------------------------------------------------
//   Globals



// -- Turn on debugging
bool debug = false;


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

  Graph g = makeGraph(Heap);
  cout << "GRAPH #: " << g.nodes.size() << endl;


  auto components = allComponents(&g);
  cout << "COMPONENT #: " << components.size() << endl;

  std::sort (components.begin(), components.end(), [](const Component& c1, const Component& c2) {
    return c1.nodes.size() > c2.nodes.size();
  });

  for (int i = 0; i < 20; i++) {
    cout << "Component " << i << " : " << components[i].nodes.size() << endl;
  }

  cout << "TRIMING" << endl << "----------------------------" << endl;
  Graph gTrim = makeGraph(Heap);
  
  cout << "BEFORE TRIM GRAPH #: " << gTrim.nodes.size() << endl;
  trimGraph(&gTrim, 10);
  cout << "AFTER TRIM GRAPH #: " << gTrim.nodes.size() << endl;

  auto componentsTrimmed = allComponents(&gTrim);
  cout << "COMPONENT #: " << componentsTrimmed.size() << endl;

  std::sort (componentsTrimmed.begin(), componentsTrimmed.end(), 
            [](const Component& c1, const Component& c2) {
    return c1.nodes.size() > c2.nodes.size();
  });


  for (int i = 0; i < 20; i++) {
    cout << "Component " << i << " : " << componentsTrimmed[i].nodes.size() << endl;
  }

}
