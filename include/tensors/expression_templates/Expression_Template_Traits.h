/*  Project: BlackCat_Tensors
 *  Author: JosephJaspers
 *  Copyright 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCAT_INTERNAL_FORWARD_DECLS_H_
#define BLACKCAT_INTERNAL_FORWARD_DECLS_H_

#include <type_traits>

namespace BC {
namespace tensors {
namespace exprs { 

template<class,class,class> struct Binary_Expression;
template<class,class>		struct Unary_Expression;

namespace detail {


template<class T> using query_system_tag = typename T::system_tag;
template<class T> using query_value_type = typename T::value_type;
template<class T> using query_allocation_type = typename T::allocation_tag;
template<class T> using query_dx_is_defined = typename T::dx_is_defined;

template<class T> using query_copy_assignable    = BC::traits::truth_type<T::copy_assignable>;
template<class T> using query_copy_constructible = BC::traits::truth_type<T::copy_constructible>;
template<class T> using query_move_assignable    = BC::traits::truth_type<T::move_assignable>;
template<class T> using query_move_constructible = BC::traits::truth_type<T::move_constructible>;

template<class T> using query_auto_broadcast = typename T::auto_broadcast;


template<class T>
struct remove_scalar_mul {
	using type = T;
	using scalar_type = typename BC::traits::conditional_detected<query_value_type, T, T>::type *;

	static T rm(T expression) {
		return expression;
	}

	//TODO fix me, a value_type should never be a reference type
	static scalar_type get_scalar(const T& expression) {
		return nullptr;
	};
};

template<class lv, class rv>
struct remove_scalar_mul<Binary_Expression<oper::Scalar_Mul, lv, rv>> {
	using type = std::conditional_t<lv::tensor_dimension == 0, rv, lv>;
	using scalar_type = std::conditional_t<lv::tensor_dimension == 0, lv ,rv>;

	static type rm(Binary_Expression<oper::Scalar_Mul, lv, rv> expression) {
		return BC::traits::constexpr_ternary<lv::tensor_dimension==0>(
				[&]() { return expression.right; },
				[&]() { return expression.left;  }
		);
	}
	static scalar_type get_scalar(Binary_Expression<oper::Scalar_Mul, lv, rv> expression) {
		return BC::traits::constexpr_ternary<lv::tensor_dimension==0>(
				[&]() { return expression.left; },
				[&]() { return expression.right;  }
		);
	}

};

template<class T>
struct remove_transpose {
	using type = T;
	static T rm(T expression) {
		return expression;
	}
};
template<class Array, class SystemTag>
struct remove_transpose<Unary_Expression<oper::transpose<SystemTag>, Array>> {
	using type = Array;

	static type rm(Unary_Expression<oper::transpose<SystemTag>, Array> expression) {
		return expression.array;
	}
};

struct select_on_dx_when_defined {
	template<class T> BCINLINE
	static auto impl(const T& array, BC::size_t index) {
		return array.dx(index);
	}
	template<class T, class... Indicies> BCINLINE
	static auto impl(const T& array, Indicies... indicies) {
		return array.dx(indicies...);
	}
};
struct select_on_dx_when_not_defined {
	template<class T> BCINLINE
	static auto impl(const T& array, BC::size_t index) {
    	return BC::traits::make_pair(array[index], 1);
	}
	template<class T, class... Indicies> BCINLINE
	static auto impl(const T& array, Indicies... indicies) {
    	return BC::traits::make_pair(array(indicies...), 1);
	}
};

}

class BC_Type  {}; //a type inherited by expressions and tensor_cores, it is used a flag and lacks a "genuine" implementation
class BC_Array {};
class BC_Expr  {};
class BC_Temporary {};
class BC_Scalar_Constant {};
class BC_Stack_Allocated {};
class BC_View {};
class BC_Noncontinuous {};
class BC_Immutable {};

//forward declare
template<int> class SubShape;
template<int,class> class Shape;


template<class T>
struct expression_traits {

	 using system_tag	  = typename BC::traits::conditional_detected<detail::query_system_tag, T, host_tag>::type;
	 using allocation_tag = typename BC::traits::conditional_detected<detail::query_allocation_type, T, system_tag>::type;
	 using value_type	  = typename BC::traits::conditional_detected<detail::query_value_type, T, void>::type;

	 static constexpr bool derivative_is_defined =
			 BC::traits::conditional_detected_t<detail::query_dx_is_defined, T, std::false_type>::value;

	 template<class... Indicies>
	 BCINLINE static auto select_on_dx(T&& expression, Indicies... indicies) {
		 using selector = std::conditional_t<derivative_is_defined,
				 detail::select_on_dx_when_defined,
				 detail::select_on_dx_when_not_defined>;
		 return selector::impl(expression, indicies...);
	 }
	 template<class... Indicies>
	 BCINLINE static auto select_on_dx(const T& expression, Indicies... indicies) {
		 using selector = std::conditional_t<derivative_is_defined,
				 detail::select_on_dx_when_defined,
				 detail::select_on_dx_when_not_defined>;
		 return selector::impl(expression, indicies...);
	 }

	static constexpr bool is_move_constructible =
					BC::traits::conditional_detected_t<
						detail::query_move_constructible, T, std::false_type>::value;

	static constexpr bool is_copy_constructible =
					BC::traits::conditional_detected_t<
						detail::query_copy_constructible, T, std::false_type>::value;

	static constexpr bool is_move_assignable 	=
					BC::traits::conditional_detected_t<
						detail::query_move_assignable, T, std::false_type>::value;

	static constexpr bool is_copy_assignable 	=
					BC::traits::conditional_detected_t<
						detail::query_copy_assignable, T, std::false_type>::value;

	static constexpr bool is_auto_broadcasted =
					BC::traits::conditional_detected_t<
						detail::query_auto_broadcast, T, std::false_type>::value;

	static constexpr bool is_bc_type  	 = std::is_base_of<BC_Type, T>::value;
	static constexpr bool is_array  	 = std::is_base_of<BC_Array, T>::value;
	static constexpr bool is_view 		 = std::is_base_of<BC_View, T>::value;
	static constexpr bool is_continuous  = !std::is_base_of<BC_Noncontinuous, T>::value;

	static constexpr bool is_expr  		     = std::is_base_of<BC_Expr, T>::value;
	static constexpr bool is_temporary 	     = std::is_base_of<BC_Temporary, T>::value;
	static constexpr bool is_stack_allocated = std::is_base_of<BC_Stack_Allocated, T>::value;
	static constexpr bool is_immutable  	 = std::is_base_of<BC_Immutable, T>::value;
};

template<class T>
struct blas_expression_traits : expression_traits<T> {

	 using remove_scalar_mul_type		= typename detail::remove_scalar_mul<T>::type;
	 using remove_transpose_type		= typename detail::remove_transpose<T>::type;
	 using remove_blas_features_type	= typename detail::remove_transpose<remove_scalar_mul_type>::type;
	 using scalar_multiplier_type		= typename detail::remove_scalar_mul<T>::scalar_type;
	 using value_type					= typename T::value_type;

	 static constexpr bool is_scalar_multiplied = !std::is_same<remove_scalar_mul_type, T>::value;
	 static constexpr bool is_transposed 		= !std::is_same<remove_transpose_type,  T>::value;

	 static remove_transpose_type remove_transpose(T expression) { return detail::remove_transpose<T>::rm(expression); }
	 static remove_scalar_mul_type remove_scalar_mul(T expression) { return detail::remove_scalar_mul<T>::rm(expression); }
	 static remove_blas_features_type remove_blas_modifiers(T expression) {
		 return detail::remove_transpose<remove_scalar_mul_type>::rm(remove_scalar_mul(expression));
	 }

	 //If an expression with a scalar, returns the scalar, else returns a nullpointer of the valuetype == to T::value_type
	static auto get_scalar(const T& expression)
		-> decltype(detail::remove_scalar_mul<T>::get_scalar(expression)) {
		return detail::remove_scalar_mul<T>::get_scalar(expression);
	}
};

} //ns BC
} //ns exprs
} //ns tensors

#endif /* BLACKCAT_INTERNAL_FORWARD_DECLS_H_ */
