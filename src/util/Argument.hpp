#ifndef __libcr_util_argument_hpp_defined
#define __libcr_util_argument_hpp_defined

#include <type_traits>

namespace cr::util
{
	template<class T>
	using copy_t = std::conditional_t<
		std::is_trivially_copyable<T>::value
		&& sizeof(T) <= sizeof(void*),
		T,
		T const&>;

	template<class T>
	using move_t = std::conditional_t<
		std::is_trivially_move_constructible<T>::value
		&& sizeof(T) <= sizeof(void*),
		T,
		T &&>;
}

#endif