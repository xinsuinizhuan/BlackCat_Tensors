/*  Project: BlackCat_Tensors
 *  Author: JosephJaspers
 *  Copyright 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKCAT_TENSOR_BASE_H_
#define BLACKCAT_TENSOR_BASE_H_

#include "Tensor_Common.h"
#include "Tensor_Operations.h"
#include "Tensor_Accessor.h"
#include "Tensor_IterAlgos.h"
#include "Tensor_Utility.h"

namespace BC {
namespace tensors {

namespace {
using namespace BC::meta;
}


template<class ExpressionTemplate>
class Tensor_Base :
        public ExpressionTemplate,
        public Tensor_Operations<ExpressionTemplate>,
        public Tensor_Utility<ExpressionTemplate>,
        public Tensor_Accessor<ExpressionTemplate>,
        public Tensor_IterAlgos<ExpressionTemplate> {

    template<class> friend class Tensor_Base;
    friend class Tensor_Operations<ExpressionTemplate>;
    friend class Tensor_Utility<ExpressionTemplate>;
    friend class Tensor_Accessor<ExpressionTemplate>;

    using parent        = ExpressionTemplate;
    using self          = Tensor_Base<ExpressionTemplate>;
    using operations    = Tensor_Operations<ExpressionTemplate>;
    using utility       = Tensor_Utility<ExpressionTemplate>;
    using accessor      = Tensor_Accessor<ExpressionTemplate>;

    using move_parameter        = only_if<exprs::expression_traits<ExpressionTemplate>::is_move_constructible, self&&>;
    using copy_parameter        = only_if<exprs::expression_traits<ExpressionTemplate>::is_copy_constructible, const self&>;
    using move_assign_parameter = only_if<exprs::expression_traits<ExpressionTemplate>::is_move_assignable,       self&&>;
    using copy_assign_parameter = only_if<exprs::expression_traits<ExpressionTemplate>::is_copy_assignable, const self&>;

public:

    using ExpressionTemplate::ExpressionTemplate;
    using ExpressionTemplate::internal;

    using ExpressionTemplate::tensor_dimension; //required
    using allocator_type = conditional_detected_t<query_allocator_type, ExpressionTemplate, void>;
    using value_type  = typename ExpressionTemplate::value_type;
	using system_tag  = typename ExpressionTemplate::system_tag;

    using operations::operator=;
	using operations::operator+;
	using operations::operator-;
	using operations::operator/;
	using operations::operator*;
	using operations::operator%;
	using operations::operator+=;
	using operations::operator-=;
	using operations::operator/=;
	using operations::operator%=;
	using operations::operator>;
	using operations::operator<;
	using operations::operator>=;
	using operations::operator<=;
	using operations::operator==;

	using accessor::operator[];
    using accessor::operator();

    Tensor_Base() = default;
    Tensor_Base(const parent&  param) : parent(param) {}
    Tensor_Base(parent&& param) : parent(param) {}

    template<class U>
    Tensor_Base(const Tensor_Base<U>&  tensor)
    : parent(tensor.as_parent()) {}


    Tensor_Base(copy_parameter tensor)
    : parent(tensor.as_parent()) {}

    Tensor_Base(move_parameter tensor)
    : parent(std::move(tensor.as_parent())) {}

    Tensor_Base& operator =(move_assign_parameter tensor) {
        this->internal_move(std::move(tensor.as_parent()));
        return *this;
    }

    Tensor_Base& operator =(copy_assign_parameter tensor) {
         operations::operator=(tensor);
         return *this;
    }

    Tensor_Base(only_if<tensor_dimension==0, value_type> scalar) {
        static_assert(tensor_dimension == 0, "SCALAR_INITIALIZATION ONLY AVAILABLE TO SCALARS");
        this->fill(scalar);
    }

    ~Tensor_Base() {
        this->deallocate();
    }

private:

    parent& as_parent() {
    	return static_cast<parent&>(*this);
    }
    const parent& as_parent() const {
    	return static_cast<const parent&>(*this);
    }
};

}
}

#endif /* TENSOR_BASE_H_ */
