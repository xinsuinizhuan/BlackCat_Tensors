/*  Project: BlackCat_Tensors
 *  Author: JosephJaspers
 *  Copyright 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef SHAPE_H_
#define SHAPE_H_

#include "Array_Base.h"
#include "Shape.h"

namespace BC {
template<class> class Evaluator;

namespace et     {
template<int dimension, class T, class allocator>
struct Array : Array_Base<Array<dimension, T, allocator>, dimension>, public Shape<dimension>, private allocator {

    using scalar_t = T;
    using allocator_t = allocator;
    using system_tag = typename allocator_t::system_tag;

    using tags = typeinfo<scalar_t, allocator_t, allocator_t>;

    __BCinline__ static constexpr int DIMS() { return dimension; }

    scalar_t* array = nullptr;
    Array() = default;

    Array(Shape<DIMS()> shape_, scalar_t* array_) : array(array_), Shape<DIMS()>(shape_) {}

    template<class U,typename = std::enable_if_t<not std::is_base_of<BC_internal_interface<U>, U>::value>>
    Array(U param) : Shape<DIMS()>(param), array(allocator::allocate(this->size())) {}

    template<class... integers, class = std::enable_if_t<MTF::seq_of<int, integers...>>>
    Array(integers... ints) : Shape<DIMS()>(ints...), array(allocator::allocate(this->size())) {}

    template<class deriv_expr, typename = std::enable_if_t<std::is_base_of<BC_internal_interface<deriv_expr>, deriv_expr>::value>>
    Array(const deriv_expr& expr) : Shape<DIMS()>(expr.inner_shape()), array(allocator::allocate(this->size())) {
        evaluate_to(*this, expr);
    }

protected:
    template<class U> Array(U param, scalar_t* array_) : array(array_), Shape<DIMS()>(param) {}
    Array(scalar_t* array_) : array(array_) {}


    void copy_construct(const Array& array_copy) {
        this->copy_shape(array_copy);
        this->array = allocator::allocate(this->size());
        evaluate_to(*this, array_copy);
    }
public:
    __BCinline__ const scalar_t* memptr() const { return array; }
    __BCinline__       scalar_t* memptr()       { return array; }

    void swap_array(Array& param) {
        std::swap(this->array, param.array);
    }

    void destroy() {
        allocator::deallocate(array);
        array = nullptr;
    }
};


//specialization for scalar --------------------------------------------------------------------------------------------------------
template<class T, class allocator>
struct Array<0, T, allocator> : Array_Base<Array<0, T, allocator>, 0>, public Shape<0>, protected allocator {

    using scalar_t = T;
    using allocator_t = allocator;
    using system_tag = typename allocator_t::system_tag;

    __BCinline__ static constexpr int DIMS() { return 0; }

    scalar_t* array = nullptr;
    Array() : array(allocator::allocate(this->size())) {}

    Array(Shape<DIMS()> shape_, scalar_t* array_) : array(array_), Shape<0>(shape_) {}

    template<class U>
    Array(U param) {
    	allocator_t::allocate(array, this->size());
    	evaluate_to(*this, param);

    }

    __BCinline__ const auto& operator [] (int index) const { return array[0]; }
    __BCinline__        auto& operator [] (int index)        { return array[0]; }

    template<class... integers> __BCinline__       auto& operator () (integers... ints) {
        return array[0];
    }

    template<class... integers> __BCinline__ const auto& operator () (integers... ints) const {
        return array[0];
    }

    __BCinline__ const scalar_t* memptr() const { return array; }
    __BCinline__       scalar_t* memptr()       { return array; }

    void copy_construct(const Array& array_copy) {
        allocator_t::allocate(array, this->size());
        evaluate_to(*this, array_copy);
    }

    void swap_array(Array& param) {
        std::swap(this->array, param.array);
    }


    void destroy() {
        allocator::deallocate(array);
        array = nullptr;
    }

};

//-----------------------------------------------type traits--------------------------------------------------------------//


template<class T> struct is_array_core_impl {
    static constexpr bool conditional = false;
};
template<int d, class T, class ml>
struct is_array_core_impl<et::Array<d, T, ml>> {
    static constexpr bool conditional = true;
};
template<class T> static constexpr bool is_array_core() { return is_array_core_impl<T>::conditional; }


template<class T>
struct BC_array_move_assignable_overrider<T, std::enable_if_t<is_array_core<T>()>> {
    static constexpr bool boolean = true;
};

template<class T>
struct BC_array_move_constructible_overrider<T, std::enable_if_t<is_array_core<T>()>> {
    static constexpr bool boolean = true;
};
template<class T>
struct BC_array_copy_constructible_overrider<T, std::enable_if_t<is_array_core<T>()>> {
    static constexpr bool boolean = true;
};
template<int d, class s, class a> struct BC_lvalue_type_overrider<Array<d,s,a>> {
    static constexpr bool boolean = false;
};


}
}

#endif /* SHAPE_H_ */
