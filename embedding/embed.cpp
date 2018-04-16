#include "embed.h"
#include <time.h>
#include <stdlib.h>

int embed_test(int npoints);

float **embed_graph(float ** d, int n) {
	Matrix<float> dist = Matrix<float>(n, n);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			dist[i][j] = d[i][j];
		}
	}
	Vector<float> w(dist.GetRows());
	Matrix<float> v(dist.GetRows(),dist.GetCols());
	dsvd(dist, n, n, w.GetRawData(), v);
	// Now, dist has matrix u
	//      w is the vector of the singular values
	//      v is the matrix v
	// Where D = US(VT) where S is the matrix with
	//   w along the diagonal.
	int first_dim = 0;
	int second_dim = 0;
	int fst = 0;
	float temp_max;
	// Find the dimensions of most significance
	// FUTURE TESTS REQUIRED - first dimension seems insignificant.
	//                         Current fix just ignores dim 0, but more examples
	//                         Needed to test
	for (int i = 1; i < n; i++) {
		if (fst == 0) {
			temp_max = w[i];
			first_dim = 0;
			fst = 1;
		} else if (temp_max < w[i]) {
			temp_max = w[i];
			first_dim = i;
		}
	}
	fst = 0;
	for (int i = 1; i < n; i++) {
		if (fst == 0 && i != first_dim) {
			temp_max = w[i];
			second_dim = 0;
			fst = 1;
		} else if (temp_max < w[i] && i != first_dim) {
			temp_max = w[i];
			second_dim = i;
		}
	}
	// Now, the two most significant sv's are first_dim and second_dim
	// We can calculate our return value, with Xij = Uij * sqrt(Sj)
	//     where i is the index of the point and j will have the two most
	//     significant dimensions as its value
	float s1 = sqrt(w[first_dim]);
	float s2 = sqrt(w[second_dim]);
	float ** pts = new float*[n];
	for (int i = 0; i < n; i++) {
		pts[i] = new float[2];
	}
	for (int i = 0; i < n; i++) {
		pts[i][0] = dist[i][first_dim] * s1;
		pts[i][1] = dist[i][second_dim] * s2;
	}
// TESTING PRINTS
/*
	printf("-----------------Testing Info-----------------\n");
	printf("First dim is %d", first_dim);
	printf("Second dim is %d", second_dim);
	printf("[");
	for (int i = 0; i < n; i++) {
		if (i != 0) {
			printf(" ");
		}
		for (int j = 0; j < n; j++) {
		       	printf("%f ", dist[i][j]);
		}
		if (i == n - 1) {
			printf("]\n");
		} else {
			printf("\n");
		}
	}

	for (int j = 0; j < n; j++) {
		printf("%f\n", sqrt(w[j]));
	}
	printf("-----------------End Test Info-----------------\n");
*/
// END OF TESTING PRINTS
	return pts;
}

#define SIZE 6
int main() {
	int arr[SIZE] = { 5, 10, 25, 50, 500, 500 };
	for (int i = 0; i < SIZE; i++) {
		embed_test(arr[i]);
	}
	return 0;
}

int embed_test(int npoints) {
	// Currently a test for the dsvd approach.
	// Four points on  a unit square, in order 0123 by index.
	printf("Setting up test\n");

	float ** d = new float*[npoints];

	for (int i = 0; i < npoints; i++) {
		d[i] = new float[npoints];
	}
	// get distances
	// randomly generates distances in range [0,100]
	srand(time(NULL));
/*
	for (int i = 1; i < npoints; i++) {
		for (int j = 0; j < i; j++) {
			d[i][j] = 100.0 * ((static_cast <float> (rand())) / (static_cast <float> (RAND_MAX)));
			d[j][i] = d[i][j];
		}
		d[i][i] = 0;
	}
*/
	float * x = new float[npoints];
	float * y = new float[npoints];
	for (int i = 0; i < npoints; i++) {
		x[i] = 100.0 * ((static_cast <float> (rand())) / (static_cast <float> (RAND_MAX)));
		y[i] = 100.0 * ((static_cast <float> (rand())) / (static_cast <float> (RAND_MAX)));
	}
	for (int i = 0; i < npoints; i++) {
		d[i][i] = 0;
		for (int j = i + 1; j < npoints; j++) {
			if (rand() * 20 == 0) {
				d[i][j] = sqrt(((x[i] - x[j]) * (x[i] - x[j])) + ((y[i] - y[j]) * (y[i] - y[j])));
				d[j][i] = d[i][j];
			}
		}
	}

	printf("Embedding\n");
	time_t time_s, time_e;
	time(&time_s);
	float ** pts = embed_graph(d, npoints);
	time(&time_e);
	printf("Done embedding\n");
	printf("Perfect on %d nodes took %f seconds\n", npoints, time_e - time_s);
	for (int i = 0; i < npoints; i++) {
		delete [] d[i];
	}
	delete [] d;
	for (int i = 0; i < npoints; i++) {
		delete pts[i];
	}
	delete [] pts;
	return(0);
}
