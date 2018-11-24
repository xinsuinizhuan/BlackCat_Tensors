/*  Project: BlackCat_Tensors
 *  Author: JosephJaspers
 *  Copyright 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef FUNCTION_DOT_H_
#define FUNCTION_DOT_H_

#include "Expression_Base.h"
#include "Internal_BLAS_Feature_Detector.h"
#include "Tree_Evaluator_Runner.h"

namespace BC {
namespace et     {
namespace oper {
template<class ml> class dot;
}
/*
 * a = M x K
 * b = K x N
 * c = M x N
 */


template<class lv, class rv, class allocator>
struct Binary_Expression<lv, rv, oper::dot<allocator>>
: Expression_Base<Binary_Expression<lv, rv,  oper::dot<allocator>>>, BLAS_FUNCTION, Shape<0> {

    using scalar_t  = typename lv::scalar_t;
    using allocator_t = allocator;

    static constexpr bool transA = blas_feature_detector<lv>::transposed;
    static constexpr bool transB = blas_feature_detector<rv>::transposed;
    static constexpr bool lv_scalar = blas_feature_detector<lv>::scalar;
    static constexpr bool rv_scalar = blas_feature_detector<rv>::scalar;
    static constexpr bool lv_eval = blas_feature_detector<lv>::evaluate;
    static constexpr bool rv_eval = blas_feature_detector<rv>::evaluate;

    static_assert(std::is_same<scalar_of<lv>, scalar_of<rv>>::value, "MATRIX MULTIPLICATION ONLY AVAILABLE TO SAME TYPE TENSORS (FLOAT/DOUBLE)");
    static_assert(lv::DIMS() == 1 && (rv::DIMS() == 1 || rv::DIMS() ==0), "DOT DIMENSION MISMATCH, INTERNAL BUG, REPORT PLEASE");
    __BCinline__ static constexpr int DIMS() { return 0; }
    __BCinline__ static constexpr int ITERATOR() { return 0; }

    lv left;
    rv right;

     Binary_Expression(lv left, rv right) : left(left), right(right) {}

template<class core, int alpha_mod, int beta_mod>
void eval(tree::injector<core, alpha_mod, beta_mod> injection_values) const {


    //get the data of the injection --> injector simply stores the alpha/beta scalar modifiers
    auto& injection = injection_values.data();

    //evaluate the left and right branches (computes only if necessary)
    auto X = CacheEvaluator<allocator>::evaluate(blas_feature_detector<lv>::get_array(left));
    auto Y = CacheEvaluator<allocator>::evaluate(blas_feature_detector<rv>::get_array(right));

    //allocate the alpha and beta scalars,
    auto alpha = allocator::static_allocate((scalar_t)alpha_mod);

    //call outer product
    allocator::dot(X.rows(), injection, X, X.leading_dimension(0), Y, Y.leading_dimension(0));

    if (lv_scalar) {
        auto alpha_lv = blas_feature_detector<lv>::get_scalar(left);
        allocator::scalar_mul(injection, alpha, alpha_lv);
    }
    if (rv_scalar) {
        auto alpha_rv = blas_feature_detector<rv>::get_scalar(right);
        allocator::scalar_mul(injection, alpha, alpha_rv);
    }
    if (beta_mod) {
        auto beta = allocator::static_allocate((scalar_t)alpha_mod);
        allocator::scalar_mul(alpha, alpha, beta);
        allocator::deallocate(beta);
    }


    //deallocate all the temporaries
    if (lv_eval) cc(X).deallocate();
    if (rv_eval) cc(Y).deallocate();
    allocator::deallocate(alpha);
}
};

}
}



#endif /* FUNCTION_DOT_H_ */