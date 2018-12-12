/*  Project: BlackCat_Tensors
 *  Author: JosephJaspers
 *  Copyright 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SHAPE_BASE_H_
#define SHAPE_BASE_H_

#include "Internal_Common.h"

namespace BC {
namespace et     {

template<class derived>
class Inner_Shape {

     const derived& as_derived() const { return static_cast<const derived&>(*this); }
           derived& as_derived()        { return static_cast<      derived&>(*this); }

     auto is() const{
        return as_derived().inner_shape();
    }
public:
     int rows() const { return is()[0]; }
     int cols() const { return is()[1]; }
     int dimension(int i) const { return is()[i]; }
     int outer_dimension() const { return is()[derived::DIMS() - 2]; }
};

template<class derived>
class Block_Shape {

     const derived& as_derived() const { return static_cast<const derived&>(*this); }
           derived& as_derived()        { return static_cast<      derived&>(*this); }

     auto bs()const {
        return as_derived().block_shape();
    }
public:
     int size() const { return bs()[derived::DIMS() - 1]; }
     int block_dimension(int i) const { return bs()[i]; }
};

template<class derived>
class Outer_Shape {

     const derived& as_derived() const { return static_cast<const derived&>(*this); }
           derived& as_derived()        { return static_cast<      derived&>(*this); }

     auto os()const {
        return as_derived().outer_shape();
    }
public:
     int leading_dimension(int i) const {
         if (derived::DIMS() == 1)
             if (i == 1)
                 return 1;
         return os()[i];
     }
};



template<class derived>
class Shape_Base
        : public Inner_Shape<derived>,
          public Block_Shape<derived> {

	static constexpr int dims() {
		return derived::DIMS();
	}
};




template<int n_dims, class InnerShape, class BlockShape>
struct Expr_Shape : Shape_Base<Expr_Shape<n_dims, InnerShape, BlockShape>> {

	InnerShape m_inner_shape;
	BlockShape m_block_shape;

	__BCinline__ constexpr int dims() { return n_dims; }
    __BCinline__ Expr_Shape(InnerShape is_, BlockShape bs_) : m_inner_shape(is_), m_block_shape(bs_) {}
    __BCinline__ int size() const { return m_inner_shape[0]; }
    __BCinline__ int rows() const { return m_inner_shape[0]; }
    __BCinline__ int cols() const { return 1; }
    __BCinline__ int dimension(int i) const { return i == 0 ? m_inner_shape[0] : 1; }
    __BCinline__ int outer_dimension() const { return m_inner_shape[0]; }
    __BCinline__ int block_dimension(int i)   const { return m_block_shape(i); }
    __BCinline__ const auto& inner_shape() const { return m_inner_shape; }
    __BCinline__ const auto& block_shape() const { return m_inner_shape; }
};

template<int dimension, class InnerShape, class BlockShape>
auto make_expr_shape(InnerShape is_, BlockShape bs_) {
	return Expr_Shape<dimension, InnerShape, BlockShape>(is_, bs_);
}


template<class T, class enabler=void>
struct is_most_likely_shape_object : std::false_type {
	using value = std::false_type;
};

template<class T>
struct is_most_likely_shape_object<T, std::enable_if_t<std::is_same<int, decltype(std::declval<T>().dims())>::value>>
: std::true_type {
	using value = std::true_type;
};


template<class T>
static constexpr bool is_shape() {
	return std::is_base_of<Shape_Base<T>, T>::value;
}

}
}


#endif /* SHAPE_BASE_H_ */
