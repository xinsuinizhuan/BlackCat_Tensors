/*
 * Defaults.h
 *
 *  Created on: Feb 22, 2018
 *      Author: joseph
 */

#ifndef DEFAULTS_H_
#define DEFAULTS_H_

#include "BlackCat_Tensors.h"
namespace BC {
namespace NN {

/*
 * This file defines the default types of tensors and which mathlibrary to use.
 * Currently only CPU lib is supported
 */

struct BASE;
using ml = CPU;
using fp_type = double;

using scal = Scalar<fp_type, ml>;
using vec = Vector<fp_type, ml>;
using mat = Matrix<fp_type, ml>;
using cube = Cube<fp_type, ml>;
using tensor4 = Tensor4<fp_type, ml>;
using tensor5 = Tensor5<fp_type, ml>;

template<class T> using _scal = Scalar<T, ml>;
template<class T> using _vec = Vector<T, ml>;
template<class T> using _mat = Matrix<T, ml>;
template<class T> using _cube = Cube<T, ml>;
template<class T> using _tensor4 = Tensor4<T, ml>;
template<class T> using _tensor5 = Tensor5<T, ml>;

}
}

#endif /* DEFAULTS_H_ */