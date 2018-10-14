/*  Project: BlackCat_Tensors
 *  Author: JosephJaspers
 *  Copyright 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef INJECTION_INFO_H_
#define INJECTION_INFO_H_

namespace BC {
namespace internal {
namespace tree {
template<class tensor_core, int alpha_modifier_ = 1, int beta_modifier_= 0>
struct injector {

	injector(tensor_core& array_) : array(array_) {}

	tensor_core& array;

	operator const tensor_core& () const { return array; }
	operator  	   tensor_core& ()       { return array; }

	const tensor_core& data() const { return array; }
		  tensor_core& data()  	    { return array; }
};
}
}
}


#endif /* INJECTION_INFO_H_ */
