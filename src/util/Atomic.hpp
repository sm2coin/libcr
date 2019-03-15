#ifndef __cr_util_atomic_hpp_defined
#define __cr_util_atomic_hpp_defined

#include <atomic>
#include "Argument.hpp"

namespace cr::util
{
	template<class T>
	/** An extension for std::atomic that also includes the `load_strong()` operation.
		`load()` has been renamed `load_weak()` to make a more obvious distinction between the two supported types of load operation. */
	class Atomic: public std::atomic<T>
	{
		using std::atomic<T>::load;
		using std::atomic<T>::operator T;
	public:
		using std::atomic<T>::atomic;

		/** Reads the globally latest value.
		@param[in] order:
			The memory order to use.
		@return
			The globally latest value. It is guaranteed that at the time of reading, there exists no later value than the one obtained. */
		inline T load_strong(
			std::memory_order order);
		/** Reads the locally latest seen value.
		@param[in] order:
			The memory order to use.
		@return
			The latest value the current thread has seen. This value is not guaranteed to be the globally latest value. */
		inline T load_weak(
			std::memory_order order);
	};
}

#include "Atomic.inl"

#endif