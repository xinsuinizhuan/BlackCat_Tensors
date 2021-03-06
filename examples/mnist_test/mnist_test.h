#include "../../include/BlackCat_Tensors.h"
#include "../../include/BlackCat_NeuralNetworks.h"
#include <chrono>

template<class System>
auto load_mnist(System system, const char* mnist_dataset, int batch_size, int samples) {

	using value_type = typename System::default_floating_point_type;
	using allocator_type = BC::Allocator<System, value_type>;
	using cube = BC::Cube<value_type, allocator_type>;

	std::ifstream read_data(mnist_dataset);
	if (!read_data.is_open()){
		std::cout << " error opening file. " << mnist_dataset << std::endl;
		throw std::invalid_argument("Failure opening mnist_train.csv");
	}

	//move the file_ptr of the csv past the headers
	std::string tmp;
	std::getline(read_data, tmp, '\n');

	int img_sz = 784; //28 * 28 grey-scale handwritten digits
	int training_sets = samples / batch_size;

	cube inputs(img_sz, batch_size, training_sets);
	cube outputs(10, batch_size, training_sets); //10 -- 1 hot vector of outputs (10 for 1 hot digits)

	//tensor's memory is uninitialized
	inputs.zero();
	outputs.zero();

	for (int i = 0; i < training_sets && read_data.good(); ++i) {
		for (int j = 0; j < batch_size && read_data.good(); ++j) {
			outputs[i][j].read_as_one_hot(read_data); //read a single int-value (comma delimited) set that index to 1
			inputs[i][j].read_csv_row(read_data);	  //read an entire row of a csv
		 }
	}

	inputs /= 255; //Normalize (images are grey-scale values ranging 0-255)
	return std::make_pair(inputs, outputs);
}

template<class System=BC::host_tag>
int percept_MNIST(System system_tag, const char* mnist_dataset,
		int epochs=5, int batch_size=32, int samples=32*1024) {

	using value_type = typename System::default_floating_point_type;
	using allocator_type = BC::Allocator<System, value_type>;
	using cube = BC::Cube<value_type, allocator_type>;
	using mat  = BC::Matrix<value_type, allocator_type>;
	using clock = std::chrono::duration<double>;

	auto network = BC::nn::neuralnetwork(
		BC::nn::feedforward(system_tag, 784, 256),
		BC::nn::tanh(system_tag, 256),
		BC::nn::feedforward(system_tag, 256, 10),
		BC::nn::softmax(system_tag, 10),
		BC::nn::logging_output_layer(system_tag, 10, BC::nn::RMSE).skip_every(100)
	);

	network.set_batch_size(batch_size);
	network.set_learning_rate(.003);

	std::cout << "Neural Network architecture: \n" <<
			network.get_string_architecture() << std::endl;

	std::pair<cube, cube> data = load_mnist(system_tag, mnist_dataset, batch_size, samples);
	cube& inputs = data.first;
	cube& outputs = data.second;

	std::cout << " training..." << std::endl;
	auto start = std::chrono::system_clock::now();
	for (int i = 0; i < epochs; ++i){
		std::cout << " current epoch: " << i << std::endl;
		for (int j = 0; j < samples/batch_size; ++j) {
			network.forward_propagation(inputs[j]);
			network.back_propagation(outputs[j]);
			network.update_weights();
		}
	}

	//	network.save("mnist_test_feedforward"); //Uncomment to add saving/loading
	//	network.load("mnist_test_feedforward");

	auto end = std::chrono::system_clock::now();
	std::cout << " training time: " <<  clock(end - start).count() << std::endl;
	{
		std::cout << " testing... " << std::endl;
		int test_images = 10;
		auto images = reshape(inputs[0], BC::shape(28,28, batch_size));
		mat hyps = network.forward_propagation(inputs[0]);
		for (int i = 0; i < test_images; ++i) {
			images[i].t().print_sparse(3);
			hyps[i].print();
			std::cout << "------------------------------------" <<std::endl;
		}
	}

	{
		std::cout << " testing... " << std::endl;
		int test_images = 10;
		auto images = reshape(inputs[0], BC::shape(28,28, batch_size));
		mat hyps = network.predict(inputs[0]);
		for (int i = 0; i < test_images; ++i) {
			images[i].t().print_sparse(3);
			hyps[i].print();
			std::cout << "------------------------------------" <<std::endl;
		}
	}

	{
		std::cout << " testing... " << std::endl;
		int test_images = 10;
		auto images = reshape(inputs[0], BC::shape(28,28, batch_size));
		for (int i = 0; i < test_images; ++i) {
			images[i].t().print_sparse(3);
			network.single_predict(inputs[0][i]).print();

			std::cout << "------------------------------------" <<std::endl;
		}
	}

	std::cout << " success " << std::endl;
	return 0;
}
