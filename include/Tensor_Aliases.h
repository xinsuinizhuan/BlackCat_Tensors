/*  Project: BlackCat_Tensors
 *  Author: JosephJaspers
 *  Copyright 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */


#ifndef TENSOR_ALIASES_H_
#define TENSOR_ALIASES_H_

#include "Tensor_Base.h"

namespace BC {

template<class T>
using Basic_Allocator = allocator::Host<T>;

template<class T> using Host = allocator::Host<T>;


#ifndef BC_GPU_DEFAULT
template<class T>
	using alloc_t = allocator::Host<T>;
#else
template<class T>
	using alloc_t = allocator::Device<T>;
#endif

#ifdef __CUDACC__
	using Cuda = allocator::Device;
	using Cuda_Managed = allocator::Device_Managed;
	template<class T> using Device = allocator::Device<T>;

	#endif

template<int dimension, class scalar_t, class allocator_t=alloc_t<scalar_t>>
using Tensor = Tensor_Base<et::Array<dimension, scalar_t, allocator_t>>;

#define BC_TENSOR_ALIAS_CORE_DEF(index, name)\
		template<class scalar_t, class allocator_t = alloc_t<scalar_t>>\
		using name = Tensor<index, scalar_t, allocator_t>;

BC_TENSOR_ALIAS_CORE_DEF(0, Scalar)
BC_TENSOR_ALIAS_CORE_DEF(1, Vector)
BC_TENSOR_ALIAS_CORE_DEF(2, Matrix)
BC_TENSOR_ALIAS_CORE_DEF(3, Cube)

template<int dimension, class scalar_t, class allocator_t=alloc_t<scalar_t>>
using Tensor_View = Tensor_Base<et::Array_View<dimension, scalar_t, allocator_t>>;

#define BC_TENSOR_ALIAS_VIEW_DEF(index, name)\
		template<class scalar_t, class allocator_t = alloc_t<scalar_t>>\
		using name##_View = Tensor_View<index, scalar_t, allocator_t>;

BC_TENSOR_ALIAS_VIEW_DEF(0, Scalar)
BC_TENSOR_ALIAS_VIEW_DEF(1, Vector)
BC_TENSOR_ALIAS_VIEW_DEF(2, Matrix)
BC_TENSOR_ALIAS_VIEW_DEF(3, Cube)

template<int dimension, class scalar_t, class allocator_t=alloc_t<scalar_t>>
using Tensor_Shared = Tensor_Base<et::Array_Shared<dimension, scalar_t, allocator_t>>;


#define BC_TENSOR_ALIAS_SHARED_DEF(index, name)\
		template<class scalar_t, class allocator_t = alloc_t<scalar_t>>\
		using name##_Shared = Tensor_Shared<index, scalar_t, allocator_t>;

BC_TENSOR_ALIAS_SHARED_DEF(0, Scalar)
BC_TENSOR_ALIAS_SHARED_DEF(1, Vector)
BC_TENSOR_ALIAS_SHARED_DEF(2, Matrix)
BC_TENSOR_ALIAS_SHARED_DEF(3, Cube)



namespace expr {
template<int x, class iterator_t, typename = std::enable_if_t<iterator_t::DIMS() == x>>
using tensor = Tensor_Base<iterator_t>;

template<class iterator_t> using scal = tensor<0, iterator_t>;
template<class iterator_t> using vec  = tensor<1, iterator_t>;
template<class iterator_t> using mat  = tensor<2, iterator_t>;
template<class iterator_t> using cube = tensor<3, iterator_t>;
}
}

#endif /* TENSOR_ALIASES_H_ */
