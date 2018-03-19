#ifndef planar_tree_discrete_h
#define planar_tree_discrete_h

#include <vector>
#include <string>

#include "rollup_tree.h"

/*********************************************************************
 * Set of types to describes points in discrete 2d space
 *********************************************************************/
using Range = std::pair<int, int>;
Range range(int r, int c);

struct Point {
  int row, col;

  Point(int r, int c) : row(r), col(c) {}

  bool operator<(const Point& p) const {
    if (row < p.row) 
      return true;
    else if (row == p.row)
      return col < p.col;
    else
      return false;     
  }

  bool operator==(const Point& p) const {
    return (row == p.row) and (col == p.col);
  }

  Point operator+(const Point& p) const {
    return Point(row + p.row, col + p.col);
  }

  bool inRange(const Point& p1, const Point& p2) {
    return p1.row <= row and row <= p2.row
       and p1.col <= col and col <= p2.col;
  }

};
std::string to_string(Point p);


/*********************************************************************
 * Node and Leaf types that make up our planar tree.
 * 
 * DPNode is an abstract base class that can be either a layer or a leaf.
 *
 * A layer represents a non terminal node, it takes up a rectangular
 *      chunk of space that contains some number of nodes.
 * In general, empty layer should not be contained in the tree.
 * 
 * A leaf is a terminal node, or a point in xy space.
 *
 * All of these are templated over type D, which represents data held by
 *      a leaf or an aggregation across a layer's children.
 *
 *********************************************************************/
template <typename D>
struct DPNode {
    virtual ~DPNode() = default;
};

template <typename D>

// Represents a square with 
// height (p0.first + dP.first) x width (p0.second + dP.second) 
struct DPLayer : DPNode<D> {
    std::vector<DPNode<D>*> children;

    Point p0;
    Point dP;

    std::string id;
    D data;

    DPLayer(Point p0_, Point dP_, std::string id_, D data_) :
         p0(p0_), dP(dP_), id(id_), data(data_) {}

};

// Leafs take up 1x1 square
template <typename D>
struct DPLeaf : DPNode<D> {
    Point p0;
    D data;

    DPLeaf(Point p0_, D data_) : p0(p0_), data(data_) {}
};


/*********************************************************************
 * Set of function that are used to convert a planar tree into a rollup
 *      tree.
 *
 * DPInfo is a struct that wrap D that contains the information needed
 *      to describe both a layer and a leaf. 
 *
 *********************************************************************/

template <typename D>
struct DPInfo {
    D data;
    Point p0;
    Point dP;
};


template <typename D>
RollupNode<DPInfo<D>>* lower(const DPNode<D>& n) {
    const DPLayer<D>* layer;
    if (layer = dynamic_cast<const DPLayer<D>*>(&n)) {
        // Process layer
        auto info = DPInfo<D> { layer->data
                              , layer->p0
                              , layer->dP
                              };
        
        auto rnode = node(layer->id, info);
        for (auto c : layer->children) {
            if (c)
                rnode->addChild(lower(*c));
        }

        return rnode;
    }

    const DPLeaf<D>* l;
    if (l = dynamic_cast<const DPLeaf<D>*>(&n)) {
        auto info = DPInfo<D> { l->data
                              , l->p0
                              , Point(0, 0)
                              };
        return leaf(info);
        // Process leaf
    }

    assert(true);

} 


/*********************************************************************
 * Converts Planar tree to a boost ptree.
 * Takes a DPNode<D>* and a function that writes D to a ptree. 
 *
 *********************************************************************/

template <typename D, typename EncodingFn>
boost::property_tree::ptree ptreeOf(const DPNode<D>& n,
                                    EncodingFn writeData) {
    auto encodeInfo = [&](const DPInfo<D>& d) {
                            boost::property_tree::ptree pt;
                            pt.put("row0",   std::to_string(d.p0.row));
                            pt.put("col0",   std::to_string(d.p0.col));
                            pt.put("dRow",   std::to_string(d.dP.row));
                            pt.put("dCol",   std::to_string(d.dP.col));
                            pt.add_child("rollup_data", writeData(d.data));
                            return pt;     
                        };

    RollupNode<DPInfo<D>>* rollup = lower(n);
    if (rollup == nullptr) {
        assert(false);
    }
    auto ptree = ptreeOf<DPInfo<D>>(*rollup, encodeInfo);
    return ptree;

}




/*********************************************************************
 * Partitions a list of DPleafs (points) into a Planar Tree.
 *
 * p0 is start of top level node and dP length of the rectangle along
 * row, col dimensions.
 *
 * Scale is how many sublayers each layer should be partitioned into.
 *
 * Aggregate is a function that describes how to turn a vector<DPNode<D>*>
 * into a D.
 *********************************************************************/
template <typename D, typename Aggregator>
DPNode<D>* partitionBy( std::vector<DPLeaf<D>*> points
                             , Point p0, Point dP
                             , Range scale
                             , Aggregator aggregate) {

  if (points.size() > 0 and dP == Point(1, 1)) {
    if (points.size() > 1) {
      std::cerr << "WARNING: multiple points at same location, discarding"
                << std::endl;
    }
    return points[0];
  } else if (not (dP == Point(1, 1))) {
    std::vector<DPNode<D>*> children;
    
    int rStep = dP.row / scale.first;
    int cStep = dP.col / scale.second;
    if (rStep == 0)
      rStep = 1;
    if (cStep == 0)
      cStep = 1;

    for (int r = p0.row; r < p0.row + dP.row; r += rStep) {
      for (int c = p0.col; c < p0.col + dP.col; c += cStep) {
        std::vector<DPLeaf<D>*> pointsInRange; 
        Point startPoint(r, c);
        Point endPoint(r + rStep - 1, c + cStep - 1);

        auto it = std::copy_if( points.begin(), points.end()
                              , std::back_inserter(pointsInRange)
                              , [&](DPLeaf<D>* l){
                                    return l->p0.inRange(startPoint, endPoint);
                                  } );

        auto node = partitionBy( pointsInRange
                               , startPoint
                               , Point(rStep, cStep)
                               , scale
                               , aggregate);

        if (node) {
          children.push_back(node);
        }
      }
    }

    if (not children.empty()) {
      auto layer = new DPLayer<D>( p0, dP
                                 , to_string(p0) + to_string(dP)
                                 , aggregate(children));
      layer->children = std::move(children);
      return layer;
    } else {
      return nullptr;
    }

  } else {
    // create nothing
    return nullptr;
  }
}



#endif // planar_tree_discrete_h