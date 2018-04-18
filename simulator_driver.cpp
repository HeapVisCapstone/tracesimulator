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

// ----------------------------------------------------------------------
//   Analysis

// void sanity_check() {
//   // -- Sanity check
  
//   const ObjectMap& fields = obj->getFields();
//   for (ObjectMap::const_iterator p = fields.begin();
//        p != fields.end();
//        p++)
//     {
//       Object * target = (*p).second;
//       if (target) {
//         if (Now > target->getDeathTime()) {
//           // -- Not good: live object points to a dead object
//           printf(" Live object %s points-to dead object %s\n",
//                  obj->info().c_str(), target->info().c_str());
//         }
//       }
//     }
//   }
  

//   /*
//   if (Now > obj->getDeathTime() && obj->getRefCount() != 0) {
//     nonzero_ref++;
//     printf(" Non-zero-ref-dead %X of type %s\n", obj->getId(),
//   obj->getType().c_str());
//   }
//   */
// }

// bool member(Object *obj, const ObjectSet &theset) {
//   return theset.find(obj) != theset.end();
// }

// int compute_roots(ObjectSet &roots) {
//   unsigned int live = 0;
//   for (ObjectMap::iterator p = Heap.begin(); p != Heap.end(); p++) {
//     Object *obj = (*p).second;
//     if (obj) {
//       if (obj->isLive(Exec.Now())) {
//         live++;
//         if (obj->getRefCount() == 0) roots.insert(obj);
//       }
//     }
//   }

//   return live;
// }

// unsigned int closure(ObjectSet &roots, ObjectSet &premarked,
//                      ObjectSet &result) {
//   unsigned int mark_work = 0;

//   vector<Object *> worklist;

//   // -- Initialize the worklist with only unmarked roots
//   for (ObjectSet::iterator i = roots.begin(); i != roots.end(); i++) {
//     Object *root = *i;
//     if (!member(root, premarked)) worklist.push_back(root);
//   }

//   // -- Do DFS until the worklist is empty
//   while (worklist.size() > 0) {
//     Object *obj = worklist.back();
//     worklist.pop_back();
//     result.insert(obj);
//     mark_work++;

//     const EdgeMap &fields = obj->getFields();
//     for (EdgeMap::const_iterator p = fields.begin(); p != fields.end(); p++) {
//       Edge *edge = (*p).second;
//       Object *target = edge->getTarget();
//       if (target) {
//         // if (target->isLive(Exec.Now())) {
//         if (!member(target, premarked) && !member(target, result)) {
//           worklist.push_back(target);
//         }
//         // } else {
//         // cout << "WEIRD: Found a dead object " << target->info() << " from "
//         // << obj->info() << endl;
//         // }
//       }
//     }
//   }

//   return mark_work;
// }

// unsigned int count_live(ObjectSet &objects, unsigned int at_time) {
//   int count = 0;
//   // -- How many are actually live
//   for (ObjectSet::iterator p = objects.begin(); p != objects.end(); p++) {
//     Object *obj = (*p);
//     if (obj->isLive(at_time)) count++;
//   }

//   return count;
// }

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

  // int i = 0;
  // for (auto c : componentsTrimmed) {
  //   cout << "Component " << i++ << " : " << c.nodes.size() << endl;
  // }
  


  // Heap.end_of_program(Exec.Now());
  // pt::ptree heap_pt = heapPtree(&Heap);

  // std::fstream jsonS;
  // jsonS.open("object.json", std::fstream::out | std::fstream::trunc);
  // pt::write_json(jsonS, heap_pt);
  // analyze();

  // jsonS.close();
}
