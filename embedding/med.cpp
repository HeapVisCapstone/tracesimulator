#include "med.h"

float find_median_asst (float * arr, int n, int i, int rank) {
	if (n <= 5) {
		float * arr_sort = new float[n];
		int marked[n];
		for (int j = 0; j < n; j++) {
			marked[j] = 0;
		}
		for (int k = 0; k < n; k++) {
			float nxt;
			int m = 1, add_j = 0;
			for (int j = 0; j < n; j++) {
				if ((m || arr[j] < nxt) && marked[j] == 0) {
					nxt = arr[j];
					m = 0;
					add_j = j;
				}
			}
			marked[add_j] = 1;
			arr_sort[k] = arr[add_j];
		}
		float ret = arr_sort[rank];
		delete [] arr_sort;
		if (i != 0) {
			delete [] arr;
		}
		return ret;
	} else {
		int j = 0, l = 0;
		int size = n / 5;
		if (n % 5 != 0) {
			size++;
		}
		float * medians = new float[size];
		while (j < n) {
			int inc;
			float * sub;
			if (n - j < 5) {
				sub = new float[n-j];
				inc = n - j;
			} else {
				sub = new float[5];
				inc = 5;
			}
			for (int k = j; k < n && k < j + 5; k++) {
				sub[k - j] = arr[k];
			}
			j += inc;
			medians[l] = find_median(sub, inc);
			free(sub);
			l++;
		}
		
		float guess = find_median(medians, size);
		delete [] medians;
		int above = 0; int below = 0; int equal = 0;
		for (int k = 0; k < n; k++) {
			if (arr[k] < guess) {
				below++;
			} else if (arr[k] > guess) {
				above++;
			} else {
				equal++;
			}
		}
		int rank_min = below;
		int rank_max = below + equal - 1;
		if (rank_min <= rank && rank_max >= rank) { // rank is good!
			if (i != 0) {
				free(arr);
			}
			return guess;
		} else if (rank_max < rank) { // rank is too low
			float * new_arr = new float[above];
			int idx = 0;
			for (int k = 0; k < n; k++) {
				if (arr[k] > guess) {
					new_arr[idx++] = arr[k];
				}
			}
			return find_median_asst(new_arr, above, 1, rank - rank_max - 1);
		} else {
			float * new_arr = new float[below];
			int idx = 0;
			for (int k = 0; k < n; k++) {
				if (arr[k] < guess) {
					new_arr[idx++] = arr[k];
				}
			}
			return find_median_asst(new_arr, below, 1, rank);
		}
	}
}

float find_median (float * arr, int n) {
	if (n == 0) {
		fprintf(stderr, "Cannot find the median of an empty set.\n");
		exit(1);
	}
	return find_median_asst(arr, n, 0, (n-1)/2);
}

float find_interval (float * arr, int n, float r) {
	if (n == 0) {
		fprintf(stderr, "Cannot find the rank of an item in an empty set.\n");
		exit(1);
	}
	if (r <= 0 || r >= 1) {
		fprintf(stderr, "Require r in (0,1)\n");
		exit(1);
	}
	return find_median_asst(arr, n, 0, (float)n * r);
}
