/*
 * Mathematics_CPU_Misc.h
 *
 *  Created on: May 6, 2018
 *      Author: joseph
 */

#ifndef MATHEMATICS_CPU_MISC_H_
#define MATHEMATICS_CPU_MISC_H_

namespace BC {

/*
 * Defines:
 *
 * 	randomize
 * 	fill
 * 	zero
 *
 */

template<class core_lib>
struct CPU_Misc {

	template<typename T, typename J>
	static void randomize(T& tensor, J lower_bound, J upper_bound) {
#ifndef BC_NO_OPENMP
#pragma omp parallel for
#endif
		for (int i = 0; i < tensor.size(); ++i) {
			tensor[i] = ((double) (rand() / ((double) RAND_MAX + 1)) * (upper_bound - lower_bound)) + lower_bound;
		}
#ifndef BC_NO_OPENMP
#pragma omp barrier
#endif
	}

	template<typename T, typename J>
	static void fill(T tensor, const J j) {
#ifndef BC_NO_OPENMP
#pragma omp parallel for
#endif
		for (int i = 0; i < tensor.size(); ++i) {
			tensor[i] = j;
		}
#ifndef BC_NO_OPENMP
#pragma omp barrier
#endif
	}
	template<typename T>
	static void zero(T tensor) {
		fill(tensor, 0);
	}

};

}

#endif /* MATHEMATICS_CPU_MISC_H_ */