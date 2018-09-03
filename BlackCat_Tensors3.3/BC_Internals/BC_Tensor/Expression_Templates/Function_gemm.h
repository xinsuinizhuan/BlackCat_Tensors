
#ifndef EXPRESSION_BINARY_GEMM_H_
#define EXPRESSION_BINARY_GEMM_H_


#include "Array_Base.h"
#include "Expression_Base.h"
#include "BlackCat_Internal_Definitions.h"
#include "Parse_Tree_BLAS_Branch_Evaluator.h"
#include "Parse_Tree_Evaluator.h"

namespace BC {
namespace oper {
template<class ml> class gemm : public BLAS_FUNCTION {};
template<class>class gemv;
}
namespace internal {

/*
 * a = M x K
 * b = K x N
 * c = M x N
 */


template<class lv, class rv, class mathlib>
struct binary_expression<lv, rv, oper::gemm<mathlib>>
: expression_base<binary_expression<lv, rv,  oper::gemm<mathlib>>>, BLAS_FUNCTION {


	using scalar_t  = typename lv::scalar_t;
	using mathlib_t = mathlib;

	static constexpr bool transA = det_eval<lv>::transposed;
	static constexpr bool transB = det_eval<rv>::transposed;
	static constexpr bool lvscalar_of = det_eval<lv>::scalar;
	static constexpr bool rvscalar_of = det_eval<rv>::scalar;
	static constexpr bool lv_eval = det_eval<lv>::evaluate;
	static constexpr bool rv_eval = det_eval<rv>::evaluate;

	static_assert(std::is_same<scalar_of<lv>, scalar_of<rv>>::value, "MATRIX MULTIPLICATION ONLY AVAILABLE TO SAME TYPE TENSORS (FLOAT/DOUBLE)");

	__BCinline__ static constexpr int DIMS() { return rv::DIMS(); }
	__BCinline__ static constexpr int ITERATOR() { return 0; }

	lv left;
	rv right;

	 binary_expression(lv left, rv right) : left(left), right(right) {}

	__BCinline__ const auto inner_shape() const { return l_array<DIMS()>([&](int i) { return i == 0 ? left.rows() : i == 1 ? right.cols() : 1; }); }

	__BCinline__ int size() const { return left.rows() * right.cols(); }
	__BCinline__ int rows() const { return left.rows(); }
	__BCinline__ int cols() const { return right.cols(); }
	__BCinline__ int dimension(int i) const { return inner_shape()[i]; }

	__BCinline__ int M() const { return left.rows();  }
	__BCinline__ int N() const { return right.cols(); }
	__BCinline__ int K() const { return left.cols();  }

	__BCinline__ auto _slice(int i) {
		return binary_expression<lv, decltype(right._slice(i)), oper::gemv<mathlib_t>>(left, right._slice(i));
	}
	__BCinline__ auto _col(int i) {
		return _slice(i);
	}

template<class core, int alpha_mod, int beta_mod>
void eval(injection_wrapper<core, alpha_mod, beta_mod> injection_values) const {

	//get the data of the injection --> injection_wrapper simply stores the alpha/beta scalar modifiers
	auto& injection = injection_values.data();

	//evaluate the left and right branches (computes only if necessary)
	auto A = branched<mathlib>::evaluate(det_eval<lv>::get_array(left));
	auto B = branched<mathlib>::evaluate(det_eval<rv>::get_array(right));

	//get the left and right side scalar values
	scalar_t* alpha_lv = det_eval<lv>::getscalar_of(left);
	scalar_t* alpha_rv = det_eval<rv>::getscalar_of(right);

	//initialize the alpha and beta scalars,
	scalar_t* alpha = mathlib::static_initialize((scalar_t)alpha_mod);
	scalar_t* beta = mathlib::static_initialize((scalar_t)beta_mod);

	//compute the scalar values if need be
	if (lvscalar_of)
		mathlib::scalar_mul(alpha, alpha, alpha_lv);
	if (rvscalar_of)
		mathlib::scalar_mul(alpha, alpha, alpha_rv);

	//call matrix_mul
	mathlib::gemm(transA, transB,  M(), N(), K(), alpha, A, A.leading_dimension(0), B, B.leading_dimension(0), beta, injection, injection.leading_dimension(0));


	//destroy all the temporaries
	if (lv_eval) cc(A).destroy();
	if (rv_eval) cc(B).destroy();
	mathlib::destroy(beta);
	mathlib::destroy(alpha);
}
};

}
}
//		if (transA)
//		std::cout << "A is transposed" << transA << std::endl;
//		if (transB)
//		std::cout <<"B is transposed" << transB << std::endl;
//		if (lvscalar_of)
//		std::cout << "A has scalar " <<lvscalar_of << std::endl;
//		if (rvscalar_of)
//		std::cout <<"B has scalar" << rvscalar_of << std::endl;
//		if (lv_eval)
//		std::cout << "A instant eval" <<lv_eval << std::endl;
//		if(rv_eval)
//		std::cout <<"B instant eval " << rv_eval << std::endl;

#endif /* EXPRESSION_BINARY_DOTPRODUCT_CU_ */