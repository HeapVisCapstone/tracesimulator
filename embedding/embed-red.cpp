#include "embed.h"
#include <time.h>
#include <stdlib.h>

using Eigen::MatrixXd;

int embed_test(int npoints);
int embed_test2(int npoints);

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

int main() {
/*	int arr[7] = { 5, 10, 25, 50, 100, 250, 500};
	int sz = 7;
	for (int i = 0; i < sz; i++) {
		embed_test(arr[i]);
	}
*/
	embed_test(10000);
	return 0;
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
