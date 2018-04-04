
#ifndef EXPRESSION_BINARY_DOTPRODUCT_CU_
#define EXPRESSION_BINARY_DOTPRODUCT_CU_

#include "Expression_Base.h"
#include "Expression_Binary_Dotproduct_impl.h"
#include "BlackCat_Internal_Definitions.h"
#include <memory>

namespace BC {

/*
 * a = M x K
 * b = K x N
 * c = M x N
 */
//det_Eval

template<class T, class lv, class rv, class Mathlib>
struct binary_expression_dotproduct : expression<T, binary_expression_dotproduct<T, lv, rv, Mathlib>> {

	struct deleter {
		template<class param>
		void operator () (param& p) {
			Mathlib::destroy(p);
		}
	};

	using scalar_type = T;
	using lifetime_reaper = std::shared_ptr<scalar_type>;

	__BCinline__ static constexpr int DIMS() { return rv::DIMS(); }
	static constexpr bool transA = det_eval<lv>::transposed;
	static constexpr bool transB = det_eval<rv>::transposed;
	static constexpr bool lv_scalar = det_eval<lv>::scalar;
	static constexpr bool rv_scalar = det_eval<rv>::scalar;
	static constexpr bool lv_eval = det_eval<lv>::evaluate;
	static constexpr bool rv_eval = det_eval<rv>::evaluate;

	lv left;
	rv right;

	lifetime_reaper array;
	scalar_type* array_ptr;

	 binary_expression_dotproduct(lv left, rv right) : left(left), right(right) {
		Mathlib::initialize(array_ptr, this->size());
		array = lifetime_reaper(array_ptr, deleter());

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

		eval();
	}

	__BCinline__ const T& operator [](int index) const  { return array_ptr[index]; }
	__BCinline__ 	   T& operator [](int index) 		{ return array_ptr[index]; }
	__BCinline__ int size() const { return left.rows() * right.cols();}
	__BCinline__ int rows() const { return left.rows(); }
	__BCinline__ int cols() const { return right.cols(); }
	__BCinline__ int dims() const { return right.dims(); }
	__BCinline__ int LD_rows() const { return rows(); }
	__BCinline__ int LD_cols() const { return size(); }
	__BCinline__ int dimension(int i)		const { return i== 0 ? rows(): i == 1 ? cols() : 1; }
	__BCinline__ const auto innerShape() 	const { return l_array([=](int i) { return i == 0 ? this->rows() : i == 1 ? this->cols() : 1; }); }
	__BCinline__ const auto outerShape() 	const { return l_array([=](int i) { return i == 0 ? this->rows() : i == 1 ? left.rows() * right.cols() : 1; }); }

	__BCinline__ int M() const { return left.rows(); }
	__BCinline__ int N() const { return right.cols(); }
	__BCinline__ int K() const { return left.cols(); }

	void printDimensions() 		const { std::cout<<"[" << M() << "][" << N()  <<"]" << std::endl; }
	void printLDDimensions()	const { std::cout<<"[" << M() << "][" << this->size()  <<"]" << std::endl; }


public:

	void eval() {

		T* A = nullptr;
		T* B = nullptr;
		T* alpha = nullptr;
		T* alpha2 = nullptr;

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
			T* tmp;
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

#endif /* EXPRESSION_BINARY_DOTPRODUCT_CU_ */
