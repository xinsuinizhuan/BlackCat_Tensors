/*  Project: BlackCat_Tensors
 *  Author: JosephJaspers
 *  Copyright 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLAS_H_
#define BLAS_H_

namespace BC   {
namespace et   {
namespace oper {

//tags, no definition
template<class allocator> struct transpose  {};
template<class allocator> struct gemm : blas_tag {};
template<class allocator> struct gemv : blas_tag {};
template<class allocator> struct ger : blas_tag {};
template<class allocator> struct dot : blas_tag {};



}
}
}



#endif /* BLAS_H_ */
