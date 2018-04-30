#include "embed.h"
#include <time.h>
#include <stdlib.h>
#include <iostream>

#define DEBUG false

using Eigen::MatrixXd;
using namespace std;

int embed_test(int npoints);

Eigen::MatrixXd embed_graph(Eigen::MatrixXd d, int n) {
	RedSVD::RedSVD<Eigen::MatrixXd> red(d, 2);
	// now, red has the U, V, matrices and S vector where
	//      S is the vectors of the singular values
	//      v is the matrix v
	// Where D = US(VT) where S is the matrix with
	//   S along the diagonal.
	Eigen::MatrixXd s(2,2);
	Eigen::VectorXd S = red.singularValues();
	s(0,0) = S(0);
	s(0,1) = 0;
	s(1,0) = 0;
	s(1,1) = S(1);
	Eigen::MatrixXd U = red.matrixU();
	Eigen::MatrixXd ret = U * s;

	return ret;
}

int embed_test(int npoints) {
	// Currently a test for the dsvd approach.
	// Four points on  a unit square, in order 0123 by index.
	printf("Setting up stest\n");

	Eigen::MatrixXd d(npoints, npoints);

	// get distances
	// randomly generates distances in range [0,100]
	srand(time(NULL));

	float * x = new float[npoints];
	float * y = new float[npoints];
	for (int i = 0; i < npoints; i++) {
		x[i] = 100.0 * ((static_cast <float> (rand())) / (static_cast <float> (RAND_MAX)));
		y[i] = 100.0 * ((static_cast <float> (rand())) / (static_cast <float> (RAND_MAX)));
	}
	for (int i = 0; i < npoints; i++) {
		d(i, i) = 0;
		for (int j = i + 1; j < npoints; j++) {
			d(i, j) = sqrt(((x[i] - x[j]) * (x[i] - x[j])) + ((y[i] - y[j]) * (y[i] - y[j])));
			d(j, i) = d(i, j);
		}
	}

	printf("Embedding\n");
	clock_t time_s, time_e;
	time_s = clock();
	Eigen::MatrixXd pts = embed_graph(d, npoints);
	time_e = clock();
	printf("Done embedding\n");
	printf("Perfect on %d nodes took %f seconds\n", npoints, ((float)time_e - (float)time_s) / CLOCKS_PER_SEC);

	return(0);
}

double *embedding_range(Eigen::MatrixXd m) {
	assert(m.cols() == 2);
	assert(m.rows() > 0);
	double *range = new double[4];
	range[0] = m(0,0);
	range[1] = m(0,0);
	range[2] = m(0,1);
	range[3] = m(0,1);
	for (int i = 1; i < m.rows(); i++) {
		double x = m(i,0);
		double y = m(i,1);
		if (x < range[0]) {
			range[0] = x;
		} else if (x > range[1]) {
			range[1] = x;
		}
		if (y < range[2]) {
			range[2] = y;
		} else if (y > range[3]) {
			range[3] = y;
		}
	}
	return range;
}

Eigen::MatrixXd shift_embedding(Eigen::MatrixXd m, double x, double y) {
	double *range = embedding_range(m);
	double x_shift = x - range[0];
	double y_shift = y - range[2];
	for (int i = 0; i < m.rows(); i++) {
		m(i,0) += x_shift;
		m(i,1) += y_shift;
	}
	return m;
}

// Helpful quicksort to sort one array and keep the other array parallel
// runs on indexes [s, e)
void parallel_quicksort(int *sort_arr, int *parallel_arr, int s, int e) {
	if (e - s < 2) {
		return;
	}
	int pivot_val = sort_arr[s];
	int parallel_pivot_val = parallel_arr[s];
	int pivot = s;
	int end = e - 1;
	while (pivot < end) {
		if (sort_arr[pivot + 1] > pivot_val) {
			int temp = sort_arr[end];
			sort_arr[end] = sort_arr[pivot + 1];
			sort_arr[pivot + 1] = temp;
			temp = parallel_arr[end];
			parallel_arr[end] = parallel_arr[pivot + 1];
			parallel_arr[pivot + 1] = temp;
			end--;
		} else {
			sort_arr[pivot] = sort_arr[pivot + 1];
			parallel_arr[pivot] = parallel_arr[pivot + 1];
			pivot++;
			sort_arr[pivot] = pivot_val;
			parallel_arr[pivot] = parallel_pivot_val;
		}
	}
	parallel_quicksort(sort_arr, parallel_arr, s, pivot);
	parallel_quicksort(sort_arr, parallel_arr, pivot + 1, e);
}

// Quick binary search, returns the index which is equal to val or the next smallest one if not equal
// left is the leftmost index, right is one larger than the rightmost index
int binary_search(int *arr, int val, int left, int right) {
	if (right - left < 2) {
		if (arr[left] <= val) {
			return left;
		} else {
			return left - 1;
		}
	}
	int idx = (left + right) / 2;
	if (arr[idx] == val) {
		return idx;
	} else if (arr[idx] < val) {
		return binary_search(arr, val, idx + 1, right);
	} else {
		return binary_search(arr, val, left, idx);
	}
}

inline int getMat(int* mat, int dim1, int x, int y) {
	return mat[dim1 * x + y];
}

inline void setMat(int* mat, int dim1, int x, int y, int value) {
	mat[dim1 * x + y] = value;
}


// build distance matrix on Component
// Note: idx[binary_search(ids, id, 0, npoints)] gives the index of the point with id 'id'.
std::pair<Eigen::MatrixXd, std::map<int, int>> build_dist_matrix(Component compnt) {
	// first, build the whole matrix.
	cout << "first, build the whole matrix." << endl;
	int nnodes = compnt.nodes.size();
	// Eigen::MatrixXd d(nnodes, nnodes);

	int *d = new int[nnodes * nnodes];
	for (int i = 0; i < nnodes; i++) {
		for (int j = 0; j < nnodes; j++) {
			setMat(d, nnodes, i, j, -1);
			// d(i,j) = -1;
		}
	}

	//
	// next, assign indices to the node id's
	//
	cout << "next, assign indices to the node id's" << endl;


	int *idx = new int[nnodes];
	int *ids = new int[nnodes];
	int i_temp = 0;
	struct Node **node_arr = new struct Node*[nnodes];
	for (auto it = compnt.nodes.begin(); it != compnt.nodes.end(); it++) {
		idx[i_temp] = i_temp;
		ids[i_temp] = (*it)->id;
		node_arr[i_temp] = *it;
		i_temp++;
	}
	parallel_quicksort(ids, idx, 0, nnodes);

	//
	// Create a mapping from new id (index in node_arr) to old object id
	//
	map<int, int> mapping;
	for (int i = 0; i < nnodes; i++) {
		mapping[i] = node_arr[i]->id;
	}


	//
	// make a new graph, identical, but with Node id = index
	//
	cout << "make a new graph, identical, but with Node id = index" << endl;
	

	std::set<struct Node *> s;
	for (int i = 0; i < nnodes; i++) {
		node_arr[i] = new Node(i);
	}


	i_temp = 0;
	for (auto node : compnt.nodes) {
		Node *target = node_arr[i_temp++];

		for (auto succ : node->successors) {
			target->successors.insert(node_arr[idx[binary_search(ids, succ->id, 0, nnodes)]]);

		}
	}

	//
	// Construct a succ map for quick access to successor indices
	//
	// This appears in memory as a 2d arrays, where row i represents the
	// successors of node i. 
	//
	// The successor ids appear unordered in the row, followed by -1 as a sentinel. 
	//
	cout << "Construct a succ map for quick access to successor indices" << endl;
	int *successors = new int[nnodes * nnodes];

	for (int i = 0; i < nnodes; i++) {
		for (int j = 0; j < nnodes; j++)
			setMat(successors, nnodes, i, j, -1);


		int j = 0;
		for (auto succ : node_arr[i]->successors) {
			setMat(successors, nnodes, i, j, succ->id);

			if (DEBUG)
				cout << "Setting : " << j << endl;
			j++;
		}

	}

	//
	// Now, run BFS on the component
	//
	cout << "Now, run BFS on the component" << endl;

	int que[nnodes];
	int unmarked[nnodes];
	int q_ins = 0;
	int q_idx = 0;
	for (int i = 0; i < nnodes; i++) {
		if (i % 1000 == 0)
			cout << "On node " << i << endl;
		for (int j = 0; j < nnodes; j++) {
			que[j] = 0;
			unmarked[j] = 1;
		}
		q_ins = 0;
		q_idx = 0;
		unmarked[i] = 0;
		que[q_ins++] = i;

		setMat(d, nnodes, i, i, 0);
		int curr_d = 0;
		while (q_idx < q_ins) {
			int curr_idx = que[q_idx];
			curr_d = getMat(d, nnodes, i, curr_idx);

			if (DEBUG) {
				cout << "poping from queue " << q_idx << " out of " << q_ins << endl;
				cout << "first succ " << getMat(successors, nnodes, curr_idx, 0) << endl;
			}

			int test_id;
			for ( int curr_succ = 0
				; (test_id = getMat(successors, nnodes, curr_idx, curr_succ)) != -1
				; curr_succ++)
			{
				if (DEBUG) {
					cout << "adding to queue : " << curr_succ << " with " << test_id << endl; 
				}
				if (unmarked[test_id]) {
					setMat(d, nnodes, i, test_id, curr_d + 1);
					unmarked[test_id] = 0;
					que[q_ins++] = test_id;
				}
			}

			q_idx++;
		}
	}
/*
	for (int i = 0; i < nnodes; i++) {
		for (int j = 0; j < nnodes; j++) {
			int test = 0;
			for (int k = 0; k < nnodes; k++) {
				if (d(i,k) == j) {
					std::set<struct Node*> sublist = node_arr[k]->successors;
					for (std::set<struct Node*>::iterator subit = sublist.begin(); subit != sublist.end();  subit++) {
						int check_idx = idx[binary_search(ids, (*subit)->id, 0, nnodes)];
						if (d(i,check_idx) == -1) {
							d(i,check_idx) = j + 1;
							test = 1;
						}
					}
				}
			}
			if (test == 0) {
				break;
			}
		}
	}
*/



	Eigen::MatrixXd distances(nnodes, nnodes);

	for (int i = 0; i < nnodes; i++)
		for (int j = 0; j < nnodes; j++)
			distances(i, j) = getMat(d, nnodes, i, j);

	// Now, d is filled s.t. every value is either a distance or -1.
	for (int i = 0; i < nnodes; i++) {
		for (int j = i + 1; j < nnodes; j++) {
			double temp = distances(i,j);
			if (temp == -1) {
				temp = distances(j, i);
			} else if (distances(j,i) < distances(i,j) && distances(j,i) != -1) {
				temp = distances(j, i);
			}
			if (temp == -1) {
				distances(i,j) = nnodes;
				distances(j,i) = nnodes;
			} else {
				distances(i,j) = temp;
				distances(j,i) = temp;
			}
		}
	}




	delete [] idx;
	delete [] ids;
	delete [] node_arr;
	delete [] d;
	delete [] successors;

	return make_pair(distances, mapping);
}


vector<Point> asPoints(const Eigen::MatrixXd &mat) {
	int n = mat.rows();

	vector<Point> points;

	for (int i = 0; i < n; i++) {
		points.push_back(Point(mat(i, 0), mat(i, 1)));
	}

	return move(points);
}

