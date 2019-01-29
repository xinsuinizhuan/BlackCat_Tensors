/*  Project: BlackCat_Tensors
 *  Author: JosephJaspers
 *  Copyright 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BC_EXPRESSION_TEMPLATES_BLAS_FEATURE_DETECTOR_H_
#define BC_EXPRESSION_TEMPLATES_BLAS_FEATURE_DETECTOR_H_

#include "Internal_Type_Traits.h"
#include "operations/BLAS.h"
#include "operations/Unary.h"
#include "operations/Binary.h"

namespace BC {
namespace et {

template<class T>           using enable_if_array = std::enable_if_t<BC::is_array<T>()>;
template<class T, class U>  using enable_if_arrays = std::enable_if_t<BC::is_array<T>() && BC::is_array<U>()>;
template<class T>           using enable_if_blas = std::enable_if_t<std::is_base_of<BLAS_FUNCTION, T>::value>;

template<class T> T&  bc_const_cast(const T&  param) { return const_cast<T&> (param); }
template<class T> T&& bc_const_cast(const T&& param) { return const_cast<T&&>(param); }
template<class T> T*  bc_const_cast(const T*  param) { return const_cast<T*> (param); }

template<class> class front;
template<template<class...> class param, class first, class... set>
class front<param<first, set...>> {
    using type = first;
};

//DEFAULT TYPE
template<class T, class voider = void> struct blas_feature_detector {
    static constexpr bool evaluate = true;
    static constexpr bool transposed = false;
    static constexpr bool scalar = false;
    static constexpr bool host_pointer_mode = true;

    template<class param> static scalar_of<param>* get_scalar(const param& p) { return nullptr; }
    template<class param> static auto& get_array (const param& p)  { return p; }
};

//IF TENSOR CORE (NON EXPRESSION)
template<class deriv> struct blas_feature_detector<deriv, enable_if_array<deriv>> {
    static constexpr bool evaluate = false;
    static constexpr bool transposed = false;
    static constexpr bool scalar = false;
    static constexpr bool host_pointer_mode = true;

    template<class param> static scalar_of<param>* get_scalar(const param& p) { return nullptr; }
    template<class param> static auto& get_array(const param& p) { return bc_const_cast(p); }
};

////IF TRANSPOSE - unary_expression(matrix^T)
template<class deriv, class ml>
struct blas_feature_detector<et::Unary_Expression<deriv, et::oper::transpose<ml>>, enable_if_array<deriv>> {
    static constexpr bool evaluate = false;
    static constexpr bool transposed = true;
    static constexpr bool scalar = false;
    static constexpr bool host_pointer_mode = true;

    template<class param> static scalar_of<param>* get_scalar(const param& p) { return nullptr; }
    template<class param> static auto& get_array(const param& p) { return bc_const_cast(p.array); }
};

////IF A SCALAR BY TENSOR MUL OPERATION - scalar * matrix
template<class d1, class d2>
struct blas_feature_detector<Binary_Expression<d1, d2, oper::scalar_mul>, enable_if_arrays<d1, d2>> {
    using self = Binary_Expression<d1, d2, oper::scalar_mul>;

    static constexpr bool evaluate = false;
    static constexpr bool transposed = false;
    static constexpr bool scalar = true;
    static constexpr bool host_pointer_mode =
    		is_scalar_constant<d1>() || is_scalar_constant<d2>();


    static constexpr bool left_scal = d1::DIMS == 0;
    static constexpr bool right_scal = d2::DIMS == 0;
    struct DISABLE;

    using left_scal_t  = std::conditional_t<left_scal,  self, DISABLE>;
    using right_scal_t = std::conditional_t<right_scal, self, DISABLE>;

    static auto&  get_array(const left_scal_t& p) { return bc_const_cast(p.right);  }
    static auto& get_array(const right_scal_t& p) { return bc_const_cast(p.left);   }
    static auto&  get_scalar(const left_scal_t& p) { return bc_const_cast(p.left);  }
    static auto& get_scalar(const right_scal_t& p) { return bc_const_cast(p.right); }
};

//IF A SCALAR BY TENSOR MUL OPERATION R + TRANSPOSED
template<class trans_t, class Scalar, class ml>
struct blas_feature_detector<Binary_Expression<Unary_Expression<trans_t, oper::transpose<ml>>, Scalar, oper::scalar_mul>, enable_if_arrays<trans_t, Scalar>> {
    static constexpr bool evaluate = false;
    static constexpr bool transposed = true;
    static constexpr bool scalar = true;

    static constexpr bool host_pointer_mode = is_scalar_constant<Scalar>();

    template<class param> static auto& get_scalar(const param& p) { return bc_const_cast(p.right); }
    template<class param> static auto& get_array(const param& p) { return bc_const_cast(p.left.array); }
};

//IF A SCALAR BY TENSOR MUL OPERATION L + TRANSPOSED
template<class Scalar, class trans_t, class ml>
struct blas_feature_detector<Binary_Expression<Scalar, Unary_Expression<trans_t, oper::transpose<ml>>, oper::scalar_mul>, enable_if_arrays<Scalar, trans_t>> {
    static constexpr bool evaluate = false;
    static constexpr bool transposed = true;
    static constexpr bool scalar = true;
    static constexpr bool host_pointer_mode = is_scalar_constant<Scalar>();

    template<class param> static auto& get_scalar(const param& p) { return bc_const_cast(p.left); }
    template<class param> static auto& get_array(const param& p) { return bc_const_cast(p.right.array); }

};
}
}
#endif /* EXPRESSION_BINARY_DOTPRODUCT_IMPL2_H_ */
