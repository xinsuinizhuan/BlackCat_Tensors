/*  Project: BlackCat_Tensors
 *  Author: JosephJaspers
 *  Copyright 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BC_EXPRESSION_TEMPLATES_FUNCTION_GEMV_H_
#define BC_EXPRESSION_TEMPLATES_FUNCTION_GEMV_H_

#include "Expression_Template_Base.h"
#include "Tree_Evaluator.h"
#include "blas_tools/Blas_tools.h"


namespace BC {
namespace tensors {
namespace exprs { 


template<class lv, class rv, class System_Tag>
struct Binary_Expression<oper::gemv<System_Tag>, lv, rv>
: Expression_Base<Binary_Expression<oper::gemv<System_Tag>, lv, rv>>, oper::gemv<System_Tag> {

	static_assert(std::is_same<typename lv::value_type, typename rv::value_type>::value,
    		"MATRIX MULTIPLICATION ONLY AVAILABLE TO SAME TYPE TENSORS (FLOAT/DOUBLE)");
    static_assert(lv::tensor_dimension == 2 && rv::tensor_dimension == 1,
    		"GEMV DIMENSION MISMATCH, INTERNAL BUG, REPORT PLEASE");

    using value_type    = typename lv::value_type;
    using system_tag  	= System_Tag;
    using blas_impl     = BC::blas::implementation<system_tag>;
    using blas_util	    = BC::tensors::exprs::blas_tools::implementation<system_tag>;

    static constexpr int tensor_dimension = 1;
    static constexpr int tensor_iterator_dimension = 1;


    lv left;
    rv right;


     Binary_Expression(lv left, rv right)
    : left(left), right(right) {}

    BCINLINE BC::size_t size() const { return left.rows(); }
    BCINLINE BC::size_t rows() const { return left.rows(); }
    BCINLINE BC::size_t cols() const { return 1; }
    BCINLINE BC::size_t dimension(int i) const { return i == 0 ? rows() : 1; }
    BCINLINE BC::size_t block_dimension(int i) const { return i == 0 ? rows() : 1; }
    BCINLINE BC::size_t M() const { return left.rows(); }
    BCINLINE BC::size_t N() const { return left.cols(); }

	BCINLINE const auto inner_shape() const {
		return utility::make_lambda_array<tensor_dimension>(
				[&](int i) {return i == 0 ? left.rows() : 1;});
	}
	BCINLINE const auto block_shape() const {
		return utility::make_lambda_array<tensor_dimension>(
				[&](int i) {return i == 0 ? rows() : 1;});
	}

    template<class core, int alpha_mod, int beta_mod, class Stream>
    void eval(injector<core, alpha_mod, beta_mod> injection_values, Stream stream) const {

		//get the data of the injection --> injector simply stores the alpha/beta scalar modifiers
		auto& injection = injection_values.data();

		//evaluate the left and right branches (computes only if necessary)
        auto contents = blas_util::template parse_expression<alpha_mod, beta_mod>(stream, left, right);
        auto A = contents.left;
        auto X = contents.right;
        auto alpha = contents.alpha;
        auto beta  = contents.beta;
        bool transA = contents.lv_is_transposed;

		blas_impl::gemv(stream, transA,  M(), N(),
				alpha.memptr(), A.memptr(), A.leading_dimension(0),
				X.memptr(), X.leading_dimension(0)/*inc_X*/,
				beta.memptr(),
				injection.memptr()/*Y*/, injection.leading_dimension(0)/*incy*/);

        blas_util::post_parse_expression_evaluation(stream, contents);

    }
};


} //ns BC
} //ns exprs
} //ns tensors


#endif /* EXPRESSION_BINARY_DOTPRODUCT_CU_ */
