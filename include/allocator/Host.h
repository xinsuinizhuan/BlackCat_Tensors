/*  Project: BlackCat_Tensors
 *  Author: JosephJaspers
 *  Copyright 2018
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BASIC_ALLOCATOR_H_
#define BASIC_ALLOCATOR_H_

namespace BC {

class host_tag;

namespace allocator {

template<class T>
struct Host {

    using size_type = int;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using is_always_equal = std::true_type;
    using propagate_on_container_move_assignment = std::true_type;

    //Black Cat-Tags
    using system_tag = host_tag;								 //host = cpu, device = cuda
    using propagate_on_expression_construction = std::true_type; //Pass the allocator to temporaries of this class
    using propagate_on_expression_type_override = Host<T>;		 //You may override this type if you want to pass a different allocator to the expressions


    T* allocate(int size) const {
        return new T[size];
    }

    T static_allocate(T value) const {
       return T(value);
    }

    void deallocate(T* ptr) const {
        delete[] ptr;
    }

    void deallocate(const T&) const {
    }

	template<class U, class V>
	void copy(T* to, U* from, V size) const {
		__BC_omp_for__
		for (int i = 0; i < size; ++i) {
			to[i] = from[i];
		}

		__BC_omp_bar__
	}

    template<class U>
    void HostToDevice(T* device_ptr, U* host_ptr, int size=1) const {
        copy(device_ptr, host_ptr, size);
    }
    template<class U>
    void DeviceToHost(T* host_ptr, U* device_ptr, int size=1) const {
        copy(host_ptr, device_ptr, size);
    }
    T extract(T* data_ptr, int index) const {
    	return data_ptr[index];
    }

};
}
}




#endif /* BASIC_ALLOCATOR_H_ */
