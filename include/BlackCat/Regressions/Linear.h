/*
 * Linear.h
 *
 *  Created on: Dec 16, 2018
 *      Author: joseph
 */

#ifndef BC_MODELS_LINEAR_H_
#define BC_MODELS_LINEAR_H_

namespace BC {
namespace models {

template<class scalar_t, class allocator=BC::Basic_Allocator<scalar_t>>
class LinearRegression {

	using cube   = BC::Cube<scalar_t, allocator>;
	using matrix = BC::Matrix<scalar_t, allocator>;
	using vector = BC::Vector<scalar_t, allocator>;

	scalar_t epsilon = .0001; //threshold amount of change to continue training
	matrix w; 		//weights
	matrix y; 		//outputs
	vector b; 		//bias
	matrix d; 		//cumulative deltas
	scalar_t lr; 	//learning rate

	const auto& get_weights() { return w; }
	const auto& get_bias()    { return b; }

	//basic check if we are initialized
	bool is_initialized() {
		return w.size();
	}

	template<class X, class Y>
	void initialize(const expr::mat<X>& inputs, const expr::mat<Y>& outputs, bool force_initialize){

		//ensure we are not just training with more data
		if (!force_initialize && is_initialized()) {
			return;
		}

		w = matrix(outputs.rows(), inputs.cols());
		b = matrix(outputs.rows());

	}

	template<class X, class Y>
	void train_instance(const X& x, Y& y) {
		y = w * x + b;					//calculate hypothesis
		y = (y - outputs) * lr;			//calculate the error (with learning rate adjustment)

		w -= y * x.t(); 				//outer product (update weights)
		b -= y;							//update bias

		delta = BC::abs(y).sum();
	}

public:
	void train(const cube& inputs, const cube& outputs, unsigned max_iters=0, bool force_initialized=false) {
		initialize(inputs[0], outputs[0], force_initialization);

		scalar_t delta = epsilon + 1;
		int batches = inputs.outer_dimension();

		if (max_iters) {
			while (delta > epsilon && max_iters) {

				for (int i = 0; i < batches; ++i) {
					train_instance(inputs[i], outputs[i]);
				}
				delta /= batches;
				max_iters--;
			}

		} else  {
			while (delta > epsilon) {
				for (int i = 0; i < batches; ++i) {
					train_instance(inputs[i], outputs[i]);
				}
				delta /= batches;
			}

		}
	}

	void train(const matrix& inputs, const matrix& outputs, unsigned max_iters=0, bool force_initialized=false) {
		initialize(inputs[0], outputs[0], force_initialization);

		scalar_t delta = epsilon + 1;

		if (max_iters) {

			while (delta > epsilon && max_iters) {
				train_instance(inputs, outputs);
				delta /= batches;
				max_iters--;
			}

		} else  {
			while (delta > epsilon) {
				train_instance(inputs, outputs);
				delta /= batches;
				max_iters--;
			}
		}
	}





};

}
}




#endif /* LINEAR_H_ */
