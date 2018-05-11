#include "components.h"
#include <assert.h>
using namespace std;

Node* removeNode(Graph* g) {
    auto begin = g->nodes.begin();
    auto node = (*begin).second;
    g->nodes.erase(begin);
    return node;
}

void addEdge(Node* n1, Node* n2) {
  n1->successors.insert(n2);
  n2->successors.insert(n1);
}

Graph makeGraph(HeapState& heap) {
  map<NodeID, Node*> nodes;

  for (EdgeSet::iterator p = heap.begin_edges();
       p != heap.end_edges();
       p++) {
    Edge* edge = *p;
    Object* source = edge->getSource();
    Object* target = edge->getTarget();

    NodeID sourceID = source->getId();
    NodeID targetID = target->getId();


    if (not nodes.count(sourceID)) {
      Node* sourceNode = new Node(sourceID);
      nodes[sourceID] = sourceNode;
    }

    if (not nodes.count(targetID)) {
      Node* targetNode = new Node(targetID);
      nodes[targetID] = targetNode;
    }

    addEdge(nodes[targetID], nodes[sourceID]);
  }

  return Graph { move(nodes) };
}


// Removes all nodes with order less equal tothan n
// Side effects other nodes to remove broken edges to trimmed nodes
void trimGraph(Graph* g, int n) {
  set<Node*> toRetain;


  for (auto node : g->nodes) {
    if (node.second->successors.size() > n) {
      toRetain.insert(node.second);
    }
  }

  map<NodeID, Node*> newNodeMap;

  for (auto node : toRetain) {
    set<Node*> newSucc;
    for (auto succ : node->successors) {
      if (contains(toRetain, succ) ) {
        newSucc.insert(succ);
      }
    }
    node->successors = newSucc;
    newNodeMap[node->id] = node;
  }

  g->nodes = move(newNodeMap);
}

// Removes all nodes with order less equal tothan n
// Side effects other nodes to remove broken edges to trimmed nodes


void filterGraph(Graph* g, Predicate p) {
  set<Node*> toRetain;


  for (auto node : g->nodes) {
    if (p(node.first))
      toRetain.insert(node.second);
  }

  map<NodeID, Node*> newNodeMap;

  for (auto node : toRetain) {
    set<Node*> newSucc;
    for (auto succ : node->successors) {
      if (contains(toRetain, succ) ) {
        newSucc.insert(succ);
      }
    }
    node->successors = newSucc;
    newNodeMap[node->id] = node;
  }

  g->nodes = move(newNodeMap);
}


// Extracts one component from graph, removes those edges and nodes from the graph
Component partitionComponent(Graph* graph) {
  assert(not graph->empty());

  set<Node*> nodes;
  
  vector<Node*> stack = { removeNode(graph) };

  while (not stack.empty()) {
    Node* current = stack.back();
    stack.pop_back();

    nodes.insert(current);
    if (graph->nodes.count(current->id)) {
        graph->nodes.erase( graph->nodes.find(current->id) );
    }
    for (auto adj_node : current->successors) {
      if (not contains(nodes, adj_node) ) {
        stack.push_back(adj_node);
      }
    }
  }

  Component c = { move(nodes) };
  return c;
}

// Removes all nodes with order less equal tothan n
void trimComponent(Component& c, int n) {
  set<Node*> toRetain;

  for (auto node : c.nodes) {
    if (node->successors.size() > n) {
      toRetain.insert(node);
    }
  }

  for (auto node : toRetain) {
    set<Node*> newSucc;
    for (auto succ : node->successors) {
      if (contains(toRetain, succ) ) {
        newSucc.insert(succ);
      }
    }
    node->successors = newSucc;
  }

  c.nodes = toRetain;
}

// Extracts all components from graph
vector<Component> allComponents(Graph* graph) {
  vector<Component> components;

  while (not graph->empty()) {
    components.push_back( partitionComponent(graph) );
  }

  return components;

}
