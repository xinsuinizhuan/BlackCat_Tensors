#include <iostream>
#include "../BlackCat_Tensors.h"
#include <type_traits>
#include "SpeedTests.h"
using BC::Vector;
using BC::Matrix;
using BC::Scalar;
using BC::Cube;

using ml = BC::CPU;
//using ml = BC::GPU;

using vec = Vector<float, ml>;
using mat = Matrix<float, ml>;
using scal = Scalar<float, ml>;
using cube = Cube<float, ml>;

auto test() {

	mat a(3, 2);
	mat b(2 ,3);
	mat d(2, 3);
	mat e(3, 2);
	mat c(2, 2);


	std::cout << " param " << std::endl;


	a.printDimensions();

	for (int i = 0; i < 6; ++i)  {
		a(i) = i + 7;
		b(i) = i + 1;
	}

	a.print();
	b.print();

	d = a.t();
	e = b.t();

	d.print();
	e.print();

	std::cout << " simple dot product [following should all have same value]" << std::endl;
	c = d * e;
	c.print();
	std::cout << " dot product -- transpose, transpose" << std::endl;
	c = a.t() * b.t();
	c.print();
	std::cout << " dot product -- transpose, regular" << std::endl;
	c = a.t() * e;
	c.print();
	std::cout << " dot product -- regular, transpose" << std::endl;
	c = d * b.t();
	c.print();
	std::cout << " dot product -- regular scalar, regular" << std::endl;
	c = d * scal(2) * e;
	c.print();
	std::cout << " dot product -- scalar regular, regular" << std::endl;
	c = scal(2) * d * e;
	c.print();
	c = d * e * scal(2); ////This is the only version that is not accounted for (it is also the least common notation)
	c.print();
	c = d * scal(2) * e;
	c.print();

	c.print();

	scal A(2);
	scal B(2);

	c.print();
	(c + c).eval();
	c = a.t() * b.t();

	A.print();

	c = a.t() * A * (b.t() * A);

	c.print();

	cube cu(2,3, 4);
	cu.zero();
	cu.print();



	std::cout << " sub vector test " << std::endl;

	d.print();
	std::cout << " adding d[1] = d[1] + d[1]" << std::endl;
	d[1] = d[1] + d[1];
	d.print();
	std::cout << " adding d[1][1] = d[1][1] + d[1][1] " << std::endl;


	d[1][1] = d[1][1] + d[1][1];
	d.print();


	std::cout << " trying to write " <<std::endl;
	std::ofstream os("save.txt");
	d.write(os);
	os.close();


	std::cout << " trying to read" << std::endl;
	std::ifstream is("save.txt");

	Matrix<double> readM(d.size());
	readM.read(is);
	readM.print();
	is.close();

}

#include "Benchmarks/BenchmarkEigen.h"



int main() {

	if (zeros('0', '0', '0'))
		std::cout << " is true " << std::endl;

	std::cout << "BENCHMARKING - 03 OPTIMIZATIONS" << std::endl;
	std::cout << "Benchmarking: " << BC_EIGEN_BENCHMARK::benchmark1_str() << std::endl;
//	omp_set_num_threads(2);
	BC_EIGEN_BENCHMARK::benchmark1<4,     100000>();
	BC_EIGEN_BENCHMARK::benchmark1<8,     100000>();
	BC_EIGEN_BENCHMARK::benchmark1<16,    10000>();
	BC_EIGEN_BENCHMARK::benchmark1<64,    10000>();
	BC_EIGEN_BENCHMARK::benchmark1<128,   1000>();
	BC_EIGEN_BENCHMARK::benchmark1<256,   1000>();
	BC_EIGEN_BENCHMARK::benchmark1<512,   100>();

	std::cout << "Benchmarking: " << BC_EIGEN_BENCHMARK::benchmark2_str() << std::endl;

	BC_EIGEN_BENCHMARK::benchmark2<4,     100000>();
	BC_EIGEN_BENCHMARK::benchmark2<8,     100000>();
	BC_EIGEN_BENCHMARK::benchmark2<16,    10000>();
	BC_EIGEN_BENCHMARK::benchmark2<64,    10000>();
	BC_EIGEN_BENCHMARK::benchmark2<128,   1000>();
	BC_EIGEN_BENCHMARK::benchmark2<256,   1000>();
	BC_EIGEN_BENCHMARK::benchmark2<512,   100>();

	std::cout << " success  main" << std::endl;

	return 0;
}