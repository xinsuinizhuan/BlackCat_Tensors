
#ifndef EXPRESSION_BINARY_DOTPRODUCT_CU_
#define EXPRESSION_BINARY_DOTPRODUCT_CU_

#include "BlackCat_Expression_Base.h" 	//Expression_Core_Base<T, binary_expression_dotproduct<T, lv, rv, Mathlib>> {
#include "BlackCat_Tensor_Core_Base.h" 	//Expression_Core_Base<T, binary_expression_dotproduct<T, lv, rv, Mathlib>> {

#include "Expression_Binary_Dotproduct_impl.h"
#include "BlackCat_Internal_Definitions.h"

namespace BC {

/*
 * a = M x K
 * b = K x N
 * c = M x N
 */
//det_Eval

template<class lv, class rv, class Mathlib>
struct binary_expression_dotproduct : Tensor_Core_Base<binary_expression_dotproduct<lv, rv, Mathlib>, rv::DIMS()> {

	using scalar_type = _scalar<lv>;


	__BCinline__ static constexpr int DIMS() { return rv::DIMS(); }
	__BCinline__ static constexpr int CONTINUOUS() { return 0; }

	static constexpr bool transA = det_eval<lv>::transposed;
	static constexpr bool transB = det_eval<rv>::transposed;
	static constexpr bool lv_scalar = det_eval<lv>::scalar;
	static constexpr bool rv_scalar = det_eval<rv>::scalar;
	static constexpr bool lv_eval = det_eval<lv>::evaluate;
	static constexpr bool rv_eval = det_eval<rv>::evaluate;

	lv left;
	rv right;

	scalar_type* array_ptr;
	int is[2] { left.rows(), right.cols() };
	int os[2] { left.rows(), left.rows() * right.cols() };

	 binary_expression_dotproduct(lv left, rv right) : left(left), right(right) {
		Mathlib::initialize(array_ptr, this->size());
		eval();
	}

	__BCinline__ const auto& operator [](int index) const  { return array_ptr[index]; }
	__BCinline__ 	   auto& operator [](int index) 	   { return array_ptr[index]; }

	__BCinline__ const auto& operator [](index_array<int[2]> index) const  { return array_ptr[index[0] + index[1] * this->LD_rows()]; }
	__BCinline__ 	   auto& operator [](index_array<int[2]> index)	   	   { return array_ptr[index[0] + index[1] * this->LD_rows()]; }


	__BCinline__ const auto innerShape() const { return is; }
	__BCinline__ const auto outerShape() const { return os; }

	__BCinline__ int M() const { return left.rows();  }
	__BCinline__ int N() const { return right.cols(); }
	__BCinline__ int K() const { return left.cols();  }

	void destroy() {
		Mathlib::destroy(array_ptr);
	}

	__BCinline__ 	   scalar_type* getIterator() 		{ return array_ptr; }
	__BCinline__ const scalar_type* getIterator() const { return array_ptr; }


public:

	void eval() {

		scalar_type* A = nullptr;
		scalar_type* B = nullptr;
		scalar_type* alpha = nullptr;
		scalar_type* alpha2 = nullptr;

		if (lv_eval) {
			Mathlib::initialize(A, left.size());
			Mathlib::copy(A, left, left.size());
		} else {
			A = det_eval<lv>::getArray(left);
		}
		if (rv_eval) {
			Mathlib::initialize(B, right.size());
			Mathlib::copy(B, right, right.size());
		} else {
			B = det_eval<rv>::getArray(right);
		}

			alpha = det_eval<lv>::getScalar(left);
			alpha2 = det_eval<rv>::getScalar(right);


		if (lv_scalar && rv_scalar){
			scalar_type* tmp;
			Mathlib::initialize(tmp, 1);
			Mathlib::scalarMul(tmp, alpha, alpha2);
			Mathlib::MatrixMul(transA, transB, A, B, array_ptr, M(), N(), K(), tmp, nullptr, left.LD_rows(), right.LD_rows(), this->rows());
			Mathlib::destroy(tmp);

		} else if (rv_scalar)
			 Mathlib::MatrixMul(transA, transB, A, B, array_ptr, M(), N(), K(), alpha2, nullptr, left.LD_rows(), right.LD_rows(), this->rows());
		 else if (lv_scalar)
			 Mathlib::MatrixMul(transA, transB, A, B, array_ptr, M(), N(), K(), alpha, nullptr, left.LD_rows(), right.LD_rows(), this->rows());
		 else
			 Mathlib::MatrixMul(transA, transB, A, B, array_ptr, M(), N(), K(), nullptr, nullptr, left.LD_rows(), right.LD_rows(), this->rows());

		if (lv_eval)
			Mathlib::destroy(A);
		if (rv_eval)
			Mathlib::destroy(B);

	}
};

}


//		if (transA)
//		std::cout << "A is transposed" << transA << std::endl;
//		if (transB)
//		std::cout <<"B is transposed" << transB << std::endl;
//		if (lv_scalar)
//		std::cout << "A has scalar " <<lv_scalar << std::endl;
//		if (rv_scalar)
//		std::cout <<"B has scalar" << rv_scalar << std::endl;
//		if (lv_eval)
//		std::cout << "A instant eval" <<lv_eval << std::endl;
//		if(rv_eval)
//		std::cout <<"B instant eval " << rv_eval << std::endl;


#endif /* EXPRESSION_BINARY_DOTPRODUCT_CU_ */