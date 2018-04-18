#include "embed.h"
#include <time.h>
#include <stdlib.h>

using Eigen::MatrixXd;

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


// build distance matrix on Component
// Note: idx[binary_search(ids, id, 0, npoints)] gives the index of the point with id 'id'.
Eigen::MatrixXd build_dist_matrix(Component compnt) {
	// first, build the whole matrix.
	int nnodes = compnt.nodes.size();
	Eigen::MatrixXd d(nnodes, nnodes);
	for (int i = 0; i < nnodes; i++) {
		for (int j = 0; j < nnodes; j++) {
			d(i,j) = -1;
		}
	}
	// next, assign indices to the node id's
	std::set<struct Node*>::iterator it;
	int *idx = new int[nnodes];
	int *ids = new int[nnodes];
	int i = 0;
	struct Node **node_arr = new struct Node*[nnodes];
	for (it = compnt.nodes.begin(); it != compnt.nodes.end(); it++) {
		idx[i] = i;
		ids[i] = it->id;
		node_arr[i] = it;
		i++;
	}
	parallel_quicksort(ids, idx, 0, nnodes);
	// now, ids is sorted and if id = ids[i], that node corresponds to idx[i] in index
	int *queue = new int[nnodes];
	for (it = compnt.nodes.begin(); it != compnt.nodes.end(); it++) {
		for (int i = 0; i < nnodes; i++) {
			queue[i] = -1;
		}
		//run a BFS on *it
		// queue_index is insertion index, curr_d is d of currently processing node,
		//   step_index is index of currently processing node in queue,
		//   step_nindex is the index of currently processing node in actual index
		int queue_index = 0, step_index = 0, step_nindex = 0;
		double curr_d = 0;
		// curr_idx is the index of current source node in actual index
		int curr_idx = idx[binary_search(ids, it->id, 0, npoints)];

		queue[queue_index++] = curr_idx;
		d(curr_idx, curr_idx) = 0;
		Node *curr_node;
		while (step_index < npoints && queue[step_index] != -1) {
			Node *curr_node = node_arr[curr_idx];
			curr_d = d(curr_idx, step_nindex);
			std::set<struct Node*> next_set = curr_node->successors;
			for (std::set<struct Node*>::iterator subit = next_set->begin(); subit < next_set->end(); subit++) {
				int temp_idx = idx[binary_search(ids, subit->id, 0, npoints)];
				double temp_d = d(curr_idx, temp_idx);
				if (temp_d == -1) {
					d(curr_idx, step_nindex) = curr_d + 1;
					queue[queue_index++] = temp_idx;
				}
			}
			if (queue_index == nnodes) {
				break;
			} else {
				step_index++;
			}
		}
	}
	// Now, d is filled s.t. every value is either a distance or -1.
	for (int i = 0; i < nnodes; i++) {
		d(i,i) = 0;
		for (int j = i + 1; j < nnodes; j++) {
			double temp = d(i,j);
			if (temp == -1) {
				temp = d(j,i);
			} else if (d(j,i) < d(i,j) && d(j,i) != -1) {
				temp = d(j,i);
			}
			if (temp == -1) {
				d(i,j) = npoints;
				d(j,i) = npoints;
			} else {
				d(i,j) = temp;
				d(j,i) = temp;
			}
		}
	}


	delete [] idx;
	delete [] ids;
	delete [] node_arr;
	delete [] queue;

	return d;
}
