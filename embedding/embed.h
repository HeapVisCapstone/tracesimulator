
#include "Eigen/Dense"
#include "RedSVD.h"
#include <iostream>
#include <assert.h>

// Receives a n-by-n distance matrix d
// Outputs a n-by-2 matrix, such that dim 1 is the vertex and dim 2 is the x- and y-coordinates
Eigen::MatrixXd embed_graph(Eigen::MatrixXd d, int n);

// returns an array of 4 values, x_min, x_max, y_min, y_max in that order
//   on the n-by-2 embedding m
double *embedding_range(Eigen::MatrixXd m);
// shift n-by-2 embedding m such that the lower left coordinate is at (x,y)
Eigen::MatrixXd shift_embedding(Eigen::MatrixXd m, double x, double y);


