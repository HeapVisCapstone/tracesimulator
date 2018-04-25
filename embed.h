#ifndef EMBED_RED_H
#define EMBED_RED_H

#include "components.h"
#include "Eigen/Dense"
#include "RedSVD/RedSVD.h"
#include <cstdio>
#include <iostream>
#include <assert.h>

#include <vector>

#include "heap.h"
#include "planar_tree_discrete.h"

// Receives a n-by-n distance matrix d
// Outputs a n-by-2 matrix, such that dim 1 is the vertex and dim 2 is the x- and y-coordinates
Eigen::MatrixXd embed_graph(Eigen::MatrixXd d, int n);

// returns an array of 4 values, x_min, x_max, y_min, y_max in that order
//   on the n-by-2 embedding m
double *embedding_range(Eigen::MatrixXd m);
// shift n-by-2 embedding m such that the lower left coordinate is at (x,y)
Eigen::MatrixXd shift_embedding(Eigen::MatrixXd m, double x, double y);

// builds a (double) distance matrix of integer distance for the set of nodes in
//   connected component 'compnt'.
// Also returns a mapping from index in return matrix to id in original heap

// - Component's "graph" has edges but not edge weights. As a result, existing edges are
//     assumed to have weight 1
// - Distance is built using SSSP. Since edge weights are equal, BFS is sufficient to get
//     SSSP from one node.
// - After running BFS from all nodes, if d(i,j) != d(j,i), set d(i,j) = d(j,i) = min(d(i,j), d(j,i))
// - Hence, distances are integers within the double matrix
std::pair<Eigen::MatrixXd, std::map<int, int>> build_dist_matrix(Component compnt);


// Converts nx2 matrix into a list of points
std::vector<Point> asPoints(const Eigen::MatrixXd &mat);

#endif
