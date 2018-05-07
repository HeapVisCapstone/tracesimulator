#include "heap_hierarchy.h"
#include <functional>
#include <iostream>
using namespace std;


namespace Recovery {
void ignoreFn(HeapState& heap,ClassHierarchy<ClassData>& ch, string s) {
    static int i = 0;
    cerr << "Not in Class Hierarchy: " << s << "  #" << i++ << endl;
}
} // Recovery

boost::property_tree::ptree writeClassData(ClassData cd) {
    boost::property_tree::ptree pt;
    pt.put("size", cd.size);
    return pt;
}

ClassData merge(ClassData x, ClassData y) {
    return ClassData(x.size + y.size);
}

ClassData aggregateUp( ClassNode<ClassData>* node) {
    ClassData agg = node->data;

    for (auto child : node->getChildren()) {
        agg = merge(agg, aggregateUp(child));
    }

    node->data = agg;


    return agg;

}


void load_heap( HeapState &heap
              , ClassHierarchy<ClassData> &ch
              , MissingIDFn missingFn) {
    ClassNode<ClassData>* node;



    for (auto obj : heap.getObjectSet()) {
        string type = obj->getType();
        if ((node = ch.getClassNode(type))) {
            node->data.size += obj->getSize();
        } else {
            missingFn(heap, ch, type);
        }
    }

    aggregateUp(ch. getRoot());

}

