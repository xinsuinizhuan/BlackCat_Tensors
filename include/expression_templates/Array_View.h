/*  Project: BlackCat_Tensors
 *  Author: JosephJaspers
 *  Copyright 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef ARRAY_VIEW_H_
#define ARRAY_VIEW_H_

#include "Array_Base.h"
#include "Array.h"

namespace BC{
namespace et     {

template<int dimension, class scalar, class allocator>
struct Array_View
        : Array_Base<Array_View<dimension, scalar, allocator>, dimension>,
          Shape<dimension> {

    using scalar_t = scalar;
    using allocator_t = allocator;
    using system_tag = typename allocator_t::system_tag;

    using tags = typeinfo<scalar, allocator, typename allocator::system_tag>;

    const scalar_t* array = nullptr;


    Array_View()                    = default;
    Array_View(const Array_View& ) = default;
    Array_View(         Array_View&&) = default;

    auto& operator = (scalar_t* move_array) {
        this->array = move_array;
        return *this;
    }

    void swap_array(Array_View& tensor) {
        std::swap(array, tensor.array);
    }

    void copy_construct(const Array_View& view) {
        this->copy_shape(view);
        this->array = view.array;
    }

    template<class tensor_t, typename = std::enable_if_t<tensor_t::DIMS() == dimension>>
    Array_View(const Array_Base<tensor_t, dimension>& tensor)
        :  array(tensor) {

        this->copy_shape(static_cast<const tensor_t&>(tensor));
    }

    template<class... integers>
    Array_View(int x, integers... ints) :Shape<dimension>(x, ints...) {}
    __BCinline__ const scalar_t* memptr() const  { return array; }

    void deallocate() {}
};
//------------------------------------------------type traits--------------------------------------------------------------//

    template<int x, class s, class a>
    struct BC_array_move_constructible_overrider<et::Array_View<x,s,a>> {
        static constexpr bool boolean = true;
    };

    template<int x, class s, class a>
    struct BC_array_copy_constructible_overrider<et::Array_View<x,s,a>> {
        static constexpr bool boolean = true; //view doesn't actually copy
    };

    template<int x, class s, class a>
    struct BC_array_move_assignable_overrider<et::Array_View<x,s,a>> {
        static constexpr bool boolean = true;
    };

    template<int x, class s, class a>
    struct BC_array_copy_assignable_overrider<et::Array_View<x,s,a>,  std::enable_if_t<is_array<et::Array_View<x,s,a>>()>> {
        static constexpr bool boolean = false;
    };
    template<int d, class s, class a> struct BC_lvalue_type_overrider<Array_View<d,s,a>> {
        static constexpr bool boolean = true;
    };



}
}

#endif /* ARRAY_VIEW_H_ */
