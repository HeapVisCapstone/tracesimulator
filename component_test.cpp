#include "embed.h"

void run_test(int n, double p);

int main() {
	run_test(2500, 0.004);
	run_test(2500, 0.01);
	run_test(2500, 0.04);
	run_test(2500, 0.1);

	return 0;
}

void run_test(int n, double p) {
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
	std::cout << (time2 - time1) / CLOCKS_PER_SEC << " seconds on n = " << n << ", p = " << p << std::endl;
	for (int i = 0; i < n; i++) {
		delete a[i];
	}
	delete [] a;
}
