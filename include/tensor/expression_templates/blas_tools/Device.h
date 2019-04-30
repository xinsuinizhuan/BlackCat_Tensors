/*
 * Device.h
 *
 *  Created on: Apr 24, 2019
 *      Author: joseph
 */

#ifndef BLACKCAT_EXPRESSION_TEMPLATES_BLAS_TOOLS_DEVICE_H_
#define BLACKCAT_EXPRESSION_TEMPLATES_BLAS_TOOLS_DEVICE_H_

#include "Device_Impl.cu"
#include "Common.h"

namespace BC {
namespace exprs {
namespace blas_tools {

struct Device : Common_Tools<Device> {

	template<class Context, class Scalar, class... Scalars>
	static void scalar_multiply(Context context, Scalar output, Scalars... vals) {
		device_impl::calculate_alpha<<<1, 1, 0, context.get_stream()>>>(output, vals...);
	}

	template<class scalar_t, int value>
	static const scalar_t* scalar_constant() {

		struct scalar_constant_initialize {
			static scalar_t* init() {
				scalar_t tmp_val = value;
				scalar_t* scalar_constant_ = nullptr;
				BC_CUDA_ASSERT(cudaMallocManaged((void**)&scalar_constant_, sizeof(scalar_t)));
				BC_CUDA_ASSERT(cudaMemcpy(scalar_constant_, &tmp_val, sizeof(scalar_t), cudaMemcpyHostToDevice));
				return scalar_constant_;
			}
		};

		static scalar_t* scalar_constant_ = scalar_constant_initialize::init();
		return scalar_constant_;
	}


};

}
}
}



#endif /* DEVICE_H_ */
