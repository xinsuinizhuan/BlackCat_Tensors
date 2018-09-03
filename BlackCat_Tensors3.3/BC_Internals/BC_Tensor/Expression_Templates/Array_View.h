/*
 * Array_View.h
 *
 *  Created on: Aug 10, 2018
 *      Author: joseph
 */

#ifndef ARRAY_VIEW_H_
#define ARRAY_VIEW_H_

#include "Array.h"

namespace BC{
namespace internal {

template<int dimension, class scalar, class allocator_t>
struct Array_View : Tensor_Array_Base<Array_View<dimension, scalar, allocator_t>, dimension>, Shape<dimension> {

	using scalar_t = scalar;
	using mathlib_t = allocator_t;
	using parent = Tensor_Array_Base<Array_View<dimension, scalar, allocator_t>, dimension>;
	using parent::parent;

	scalar_t* array = nullptr;

	Array_View() 				   = default;
	Array_View(const Array_View& ) = default;
	Array_View(		 Array_View&&) = default;

	void swap_array(Array_View& tensor) {
		std::swap(array, tensor.array);
	}

	template<class tensor_t, typename = std::enable_if_t<is_array<tensor_t>() && tensor_t::DIMS() == dimension>>
	Array_View(const Tensor_Array_Base<tensor_t, dimension>& tensor)
	:  array(const_cast<tensor_t&>(static_cast<const tensor_t&>(tensor)).memptr()) {
		this->copy_shape(static_cast<const tensor_t&>(tensor));
	}
	__BCinline__ const scalar_t* memptr() const  { return array; }
	__BCinline__ 	   scalar_t* memptr() 	     { return array; }



	void destroy() {}
};

}
	template<int x, class s, class a>
	struct BC_array_move_constructible_overrider<internal::Array_View<x,s,a>> {
		static constexpr bool boolean = true;
	};

	template<int x, class s, class a>
	struct BC_array_copy_constructible_overrider<internal::Array_View<x,s,a>> {
		static constexpr bool boolean = true;
	};

	template<int x, class s, class a>
	struct BC_array_move_assignable_overrider<internal::Array_View<x,s,a>> {
		static constexpr bool boolean = true;
	};

//	template<int x, class s, class a>
//	struct BC_array_copy_assignable_overrider<internal::Array_View<x,s,a>> {
//		static constexpr bool boolean = true;
//	};

}

#endif /* ARRAY_VIEW_H_ */