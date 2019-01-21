/*
 * linear_regression.h
 *
 *  Created on: Jan 21, 2019
 *      Author: joseph
 */

#ifndef LINEAR_REGRESSION_H_
#define LINEAR_REGRESSION_H_

template<
	class scalar_t,
	class allocator_t>
std::pair<BC::Vector<scalar_t, allocator_t>, BC::Scalar<scalar_t, allocator_t>>
linear_regression(const BC::Matrix<scalar_t, allocator_t>& data, const BC::Vector<scalar_t, allocator_t>& outputs ) {

	using scalar = BC::Scalar<scalar_t, allocator_t>;
	using vec = BC::Vector<scalar_t, allocator_t>;

	scalar_t episolon = .10;	//once delta is below this threshold we have 'converged'
	scalar_t delta    = 1;

	vec error(data.cols());
	vec weights(data.rows());
	scalar bias = 0;

	while (delta > episolon) {

		 error = outputs - (data.t() * weights + bias);
		 delta = error.sum() / (scalar_t)outputs.size();

		 bias -= delta;
		 weights -= delta;
	}

	return std::make_pair(weights, bias);
}


template<
	class scalar_t,
	class allocator_t=BC::Basic_Allocator<scalar_t>>
int linear_regression_example() {

	using scalar = BC::Scalar<scalar_t, allocator_t>;
	using vec = BC::Vector<scalar_t, allocator_t>;
	using mat = BC::Matrix<scalar_t, allocator_t>;

	int n_weights = 10;
	int n_data_points = 10;

	vec w(n_weights);

	scalar_t bias = rand() % 10; 	//randomize range of 0,10
	w.randomize(0, 10);     		//randomize range of 0,10

	mat inputs(n_weights, n_data_points);
	inputs.randomize(0, 10);

	vec outputs(n_data_points);
	outputs = inputs * w;

	std::cout << "The weights are: \n";
	w.print();

	std::cout << "The bias is: " << bias << std::endl;

	auto output = linear_regression(inputs, outputs);
	std::cout << "The predicted weighs are: \n";
	output.first.print();
	std::cout << "The predicted bias is: " << std::endl;
	output.second.print();



}

#endif /* LINEAR_REGRESSION_H_ */
