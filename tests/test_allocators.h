/*
 * test_allocators.h
 *
 *  Created on: Jan 13, 2019
 *      Author: joseph
 */

#ifndef TEST_ALLOCATORS_H_
#define TEST_ALLOCATORS_H_

#include "test_common.h"
#include <memory>

namespace BC {
namespace tests {


template<class allocator>
struct log_allocator : allocator {

	std::shared_ptr<BC::size_t> total_allocated = std::shared_ptr<BC::size_t>(new BC::size_t());
	std::shared_ptr<BC::size_t> total_deallocated = std::shared_ptr<BC::size_t>(new BC::size_t());

	template<class T>
	struct rebind {
		using other = log_allocator<typename allocator::template rebind<T>::other>;
	};


	log_allocator() = default;

	template<class T>
	log_allocator(const log_allocator<T>& la) {
		total_allocated = la.total_allocated;
		total_deallocated = la.total_deallocated;
	}

	auto allocate(BC::size_t sz) {
		(*total_allocated.get()) += sz * sizeof(typename allocator::value_type);
		return allocator::allocate(sz);
	}
	auto deallocate(typename allocator::value_type*& memptr, BC::size_t sz) {
		(*total_deallocated.get()) += sz* sizeof(typename allocator::value_type);
		return allocator::deallocate(memptr, sz);

	}
};

template<class value_type, template<class> class allocator>
int test_allocators(int sz=128) {

	using BC::tensors::all;

	BC_TEST_BODY_HEAD


	using mat = BC::Matrix<value_type, log_allocator<allocator<value_type>>>;
	using vec = BC::Vector<value_type, log_allocator<allocator<value_type>>>;

	using system_tag = typename allocator_traits<allocator<value_type>>::system_tag;

	//A stream by default references the default stream and the default global memory_pool
	//(Use 'create_stream' to initialize a new stream and a new memory_pool
	Stream<system_tag> stream;

	stream.get_allocator().free();
	stream.get_allocator().set_allocator(log_allocator<allocator<value_type>>());


	mat tmp;
	tmp.get_stream().get_allocator().set_allocator(log_allocator<allocator<value_type>>());

	BC_TEST_DEF(
		mat a(5,5);
		return *(a.get_allocator().total_allocated.get()) == 25 * sizeof(value_type);
	)

	BC_TEST_DEF(
		mat a(5,5);
		mat b(a);
		return *(b.get_allocator().total_allocated.get()) == 50 * sizeof(value_type);
	)


	//Allocators can be passed from slices
	BC_TEST_DEF(
		mat a(5,5);  //mem sz = 25
		vec b(a[0]); //       = 30 (allocators should propagate from slices)

		return *(b.get_allocator().total_allocated.get()) == 30 * sizeof(value_type);
	)

// Test no longer relevant
//	//Allocators can be passed from slices
//	BC_TEST_DEF(
//		mat a(5,5);  //mem sz = 25
//		a.get_stream().get_allocator().reserve(25 * sizeof(value_type));
//
//		a %= a * a;	 //25 memory should be allocated for the a*a temporary
//					 //25 should be deallocated after using the a*a temporary
//
//		return *(a.get_allocator().total_allocated.get()) == 25  * sizeof(value_type) &&
//				*(a.get_allocator().total_deallocated.get()) == 25  * sizeof(value_type);
//	)

	BC_TEST_DEF(
		mat a(5,5);  //mem sz = 25
		a.get_stream().get_allocator().reserve(30 * sizeof(value_type));
		a = BC::logistic(a * a + a); // should not allocate any memory
		return *(a.get_allocator().total_allocated.get()) == 25 * sizeof(value_type);
	)


// This test is no longer relevant
//	BC_TEST_DEF(
//		mat a(5,5);  //mem sz = 25
//		a.alias() = BC::logistic(a * a + a); //alias should disable expression re-ordering
//		return *(a.get_allocator().total_allocated.get()) == 50  * sizeof(value_type);
//	)

	BC_TEST_BODY_TAIL
};





}
}



#endif /* TEST_ALLOCATORS_H_ */
