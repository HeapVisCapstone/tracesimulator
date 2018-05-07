#ifndef HEAP_HIERARCHY_H
#define HEAP_HIERARCHY_H

#include "class_hierarchy.h"
#include "heap.h"
#include <functional>
#include <boost/property_tree/ptree.hpp>


struct ClassData {
    ClassData() : size(0) {}
    ClassData(int i) : size(i) {}

    int size;
};


boost::property_tree::ptree writeClassData(ClassData cd);


using MissingIDFn = function<void(HeapState&, ClassHierarchy<ClassData>&, string)>;


namespace Recovery {
void ignoreFn(HeapState& heap,ClassHierarchy<ClassData>& ch, std::string s);

} // Recovery


void load_heap(HeapState& heap, ClassHierarchy<ClassData>& ch, MissingIDFn missingFn);


#endif // HEAP_HIERARCHY_H