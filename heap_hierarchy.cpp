#include "heap_hierarchy.h"
#include <functional>

using namespace std;

using MissingIDFn = function<void(HeapState&, ClassHierarchy<ClassData>&, string)>;

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

}

