#include "embed.h"

void run_test(int n, double p, int print_d, int verif);
int verif_test(Eigen::MatrixXd d, int n);

int main() {
	// run_test(500, 0.1, 0, 1);
	// run_test(800, 0.1, 0, 1);
	
	// run_test(2500, 0.004, 0, 0);
	// run_test(2500, 0.01, 0, 0);
	// run_test(2500, 0.04, 0, 0);
	// run_test(2500, 0.1, 0, 0);

	run_test(10000, 0.004, 0, 0);


	return 0;
}

// Includes:
//  print_d: A switch for printing the distance matrix
//  verif: A switch for verifying the results of build_dist_matrix
//     - Does not test correctness, tests properties of the distance matrix:
//       + symmetric
//       + d(i,i) = 0
//       + d(i,j) = min over k {d(i,k) + d(k,j)}
//     - Note verif is strictly V^3, so it should not be used for n > 2000
void run_test(int n, double p, int print_d, int verif) {
	srand(time(NULL));
	struct Component c;
	std::set<struct Node*> s;
	c.nodes = s;
	struct Node **a = new Node *[n];
	for (int i = 0; i < n; i++) {
		a[i] = new Node(i);
		c.nodes.insert(a[i]);
	}
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i != j) {
				double r = ((double)rand() / (double)RAND_MAX);
				if (r < p) {
					a[i]->successors.insert(a[j]);
				}
			}
		}
	}
	clock_t time1 = clock();
	Eigen::MatrixXd d = build_dist_matrix(c);
	clock_t time2 = clock();
	if (print_d) {
		std::cout << d << std::endl;
	}
	std::cout << (time2 - time1) / CLOCKS_PER_SEC << " seconds on n = " << n << ", p = " << p << std::endl;
	if (verif) {
		std::cout << verif_test(d,n) << " errors" << std::endl;
	}
	for (int i = 0; i < n; i++) {
		delete a[i];
	}
	delete [] a;
}

// Counts violations as noted earlier
int verif_test(Eigen::MatrixXd d, int n) {
	int ret = 0;
	// test 0 diagonal
	for (int i = 0; i < n; i++) {
		if (d(i,i) != 0) {
			ret++;
		}
	}
	// test symmetry
	for (int i = 0; i < n; i++) {
		for (int j = i; j < n; j++) {
			if (d(i,j) != d(j,i)) {
				ret++;
			}
		}
	}
	// test distance invariant
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i != j && d(i,j) != n) {
				for (int k = 0; k < n; k++) {
					int min = 2 * n;
					if (k != i && k != j) {
						if (d(i,k) + d(k,j) < min) {
							min = d(i,k) + d(k,j);
						}
					}
					if (min != d(i,j)) {
						ret++;
					}
				}
			}
		}
	}
}
