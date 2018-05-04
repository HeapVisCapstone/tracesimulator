#ifndef HEAP_HIERARCHY_H
#define HEAP_HIERARCHY_H

#include "class_hierarchy.h"
#include "heap.h"


struct ClassData {
    ClassData() : size(0) {}
    int size;
};

void load_heap(HeapState heap, ClassHierarchy<ClassData> ch);


#endif // HEAP_HIERARCHY_H