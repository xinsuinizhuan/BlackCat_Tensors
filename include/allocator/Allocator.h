/*
 * Allocators.h
 *
 *  Created on: Dec 4, 2018
 *      Author: joseph
 */

#ifndef ALLOCATORS_H_
#define ALLOCATORS_H_

#include "Host.h"
#include "Device.h"
#include "Device_Managed.h"

namespace BC {

class host_tag;
class device_tag;

namespace allocator {

#ifdef __CUDACC__
	template<class system_tag, class scalar_t>
	using implementation =
			std::conditional_t<
				std::is_same<host_tag, system_tag>::value,
					Host<scalar_t>,
					Device<scalar_t>>;
#else
	template<class system_tag, class scalar_t>
	using implementation = Host<scalar_t>;
#endif

}
}



#endif /* ALLOCATORS_H_ */
