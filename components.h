#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <set>
#include <algorithm>
#include <iterator>

#include "heap.h"
using namespace std;


typedef unsigned int NodeID;

struct Node {
    NodeID id;
    set<struct Node*> successors;

    Node(NodeID _id) {
        id = _id;
    }

    // bool operator <(const Node &b) const {
    //     return id < b.id;
    // }
};

struct Component {
    set<struct Node*> nodes;
};



struct Graph {
    map<NodeID, struct Node*> nodes;

    bool empty() {
        return nodes.empty();
    }
};


Graph makeGraph(HeapState& heap);

// Removes all nodes with order less equal tothan n
// Side effects other nodes to remove broken edges to trimmed nodes
void trimGraph(Graph* g, int n);

// Extracts one component from graph, removes those edges and nodes from the graph
Component partitionComponent(Graph* graph);

// Extracts all components from graph
vector<Component> allComponents(Graph* graph);



// https://stackoverflow.com/questions/1701067/how-to-check-that-an-element-is-in-a-stdset
template<class TInputIterator, class T> inline
bool contains(TInputIterator first, TInputIterator last, const T& value)
{
    return std::find(first, last, value) != last;
}

template<class TContainer, class T> inline
bool contains(const TContainer& container, const T& value)
{
    // This works with more containers but requires std::begin and std::end
    // from C++0x, which you can get either:
    //  1. By using a C++0x compiler or
    //  2. Including the utility functions below.
    return contains(std::begin(container), std::end(container), value);

    // This works pre-C++0x (and without the utility functions below, but doesn't
    // work for fixed-length arrays.
    //return contains(container.begin(), container.end(), value);
}

template<class T> inline
bool contains(const std::set<T>& container, const T& value)
{
    return container.find(value) != container.end();
}


#endif // COMPONENTS_H