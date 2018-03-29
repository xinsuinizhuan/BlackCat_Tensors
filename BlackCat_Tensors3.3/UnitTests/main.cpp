#include <iostream>
#include "../BlackCat_Tensors.h"
#include <type_traits>
#include "SpeedTests.h"
#include "dotproduct_scratch.h"
#include <omp.h>
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
	scal ALPHAS(2);

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

	std::cout << " E" << std::endl;
	e.print();
	std::cout << " a.print()" << std::endl;
	a.print();
	(a[0][0] + a).print();
	(a[0][0] + a + e  + ALPHAS)[0].print(); //Only the first column is calculated (the beauty of delayed evaluation)
//
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
	std::cout << " dot product -- scalar, regular" << std::endl;
	c = d * scal(2) * e;
	c.print();
	std::cout << " dot product -- scalar, regular" << std::endl;
	c = scal(2) * d * e;
	c.print();
	std::cout << " dot product -- regular,  scalar " << std::endl;

	c = d * e * scal(2); ////This is the only version that is not accounted for (it is also the least common notation)
	c.print();
	std::cout << " dot product -- scalar, regular " << std::endl;
	c = d * scal(2) * e;
	c.print();

	scal A(2);
	scal B(2);

	std::cout << " dot product -- trans, trans " << std::endl;
	c = a.t() * b.t();
	c.print();

	std::cout << " dot product -- trans scalar, trans " << std::endl;
	A.print();
	c = a.t() * A * b.t();
	c.print();

	std::cout << " dot product -- scalar trans, trans " << std::endl;
	A.print();
	c = A * a.t() * b.t();
	c.print();

	std::cout << " dot product -- trans, trans scalar " << std::endl;
	A.print();
	c = a.t() * (b.t() * A);
	c.print();

	std::cout << " dot product -- trans scalar, trans scalar " << std::endl;
	A.print();
	c = a.t() * A * (b.t() * A);
	c.print();




	std::cout << " PRINTING CUBE EXAMPLE FORMAT " << std::endl;
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

template<int a> struct X {
	template<int> struct Y {

	};

	Y<a> getY();
};

template<class> struct Base;
template<int x>
template<int y>
struct Base<typename X<x>::template Y<y>> {
	static void foo() {
		std::cout << "special " << std::endl;
	}
};
template<int a, int b>
	using type = typename X<a>::template Y<b>;
int main() {

//	Base<type<0,0>>::foo();
//	Base<typename X<0>::template Y<0>>::foo();

test();

//BC::Dimension<4>::Tensor<double, BC::CPU> meh(std::vector<int>{1,2,3,4});
//meh.print();
//chk<2, X<2>>::foo();

	BC::Tensor4<float, BC::CPU> t4(1,2,3,4);
	t4.print();
//	var.print();
mat alpha(2,2);
mat zeta(4,4);

for (int i = 0; i < alpha.size(); ++i ){
	alpha(i) = i;
}
for (int i = 0; i < zeta.size(); ++i ){
	zeta(i) = i;
}

alpha.print();
zeta.print();
mat output(3,3);
////cube output_padded(5,5, 5);
//
//output = alpha.x_corr(zeta);
output.print();
//BC::Tensor<4, double> tens = {1,2,3,4,5};
//
//	std::cout << "BENCHMARKING - 03 OPTIMIZATIONS" << std::endl;
////	std::cout << "Benchmarking: " << BC_EIGEN_BENCHMARK::benchmark1_str() << std::endl;
//////	omp_set_num_threads(2);
//	BC_EIGEN_BENCHMARK::benchmark1<4,     100000>();
//	BC_EIGEN_BENCHMARK::benchmark1<8,     100000>();
//	BC_EIGEN_BENCHMARK::benchmark1<16,    10000>();
//	BC_EIGEN_BENCHMARK::benchmark1<64,    10000>();
//	BC_EIGEN_BENCHMARK::benchmark1<128,   1000>();
//	BC_EIGEN_BENCHMARK::benchmark1<256,   1000>();
//	BC_EIGEN_BENCHMARK::benchmark1<512,   100>();
//
//	std::cout << "Benchmarking: " << BC_EIGEN_BENCHMARK::benchmark2_str() << std::endl;
//
//	BC_EIGEN_BENCHMARK::benchmark2<4,     100000>();
//	BC_EIGEN_BENCHMARK::benchmark2<8,     100000>();
//	BC_EIGEN_BENCHMARK::benchmark2<16,    10000>();
//	BC_EIGEN_BENCHMARK::benchmark2<64,    10000>();
//	BC_EIGEN_BENCHMARK::benchmark2<128,   1000>();
//	BC_EIGEN_BENCHMARK::benchmark2<256,   1000>();
//	BC_EIGEN_BENCHMARK::benchmark2<512,   100>();
//////
	std::cout << " success  main" << std::endl;
//
//	return 0;
}
