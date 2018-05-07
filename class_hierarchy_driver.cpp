#include <iostream>
#include <fstream>

#include "simulator.h"
#include "class_hierarchy.h"
#include "heap_hierarchy.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "rollup_tree.h"

using namespace std;

namespace pt = boost::property_tree;


// USAGE
// ./class_hierarchy_driver [file].hierarchy [file].names [outfile].json < [file].trace

int main(int argc, char *argv[]) {

    if (argc < 4) {
        cerr << "Usage: class_hierarchy_driver [file].hierarchy [file].names [outfile].json < [file].trace" << endl;
    }
    string hierarchyFile(argv[1]);
    string nameFile(argv[2]);
    
    ofstream outfile;
    outfile.open(argv[3]);
    

    cout << "hello world" << endl;

    std::ifstream fs;
    fs.open (hierarchyFile, std::ifstream::in);


    auto h = ClassHierarchy<ClassData>::fromDumpFile(fs, true);

    cout << "Tree size " << h.size() << endl;

    h.print();

    cout << "BUILDING FROM HEAP" << endl;
    FILE *f = fdopen(0, "r");
    auto pair = read_trace_file(nameFile.c_str(), f);
    HeapState heap = pair.first;
    ExecState exec = pair.second;

    load_heap(heap, h, Recovery::ignoreFn);

    RollupNode<ClassData>* rollup = h.asRollupNode();

    auto ptree = ptreeOf<ClassData>(*rollup, writeClassData);

    write_json(outfile, ptree);


}
