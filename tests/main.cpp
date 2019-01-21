

#include <memory>
#include "test_accessors.h"
#include "test_allocators.h"
#include "test_common.h"
#include "test_constructors.h"
#include "test_operations.h"

template<class scalar_type, template<class> class allocator>
int test_all(int sz) {

	using namespace BC::tests;

	int errors = 0;
	errors += test_accessors<scalar_type, allocator>(sz);
	errors += test_allocators<scalar_type, allocator>(sz);
	errors += test_accessors<scalar_type, allocator>(sz);
	errors += test_constructors<scalar_type, allocator>(sz);
	errors += test_operations<scalar_type, allocator>(sz);

	return errors;
}


int main() {

	int errors = 0;
	int sz = 128;

	errors += test_all<double, BC::Basic_Allocator>(128);
	errors += test_all<double, std::allocator>(128);

#ifdef __CUDACC__ //remember to change filename to main.cu
	errors += test_all<float, BC::Cuda>(128);
#endif

	return errors;
}
