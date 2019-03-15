#ifndef __libcr_detail_nextpointer_hpp_defined
#define __libcr_detail_nextpointer_hpp_defined

#include "../util/Atomic.hpp"

namespace cr
{
	class Coroutine;
}

namespace cr::detail
{
	/** Thread-safe version of the next waiting coroutine reference. */
	class AtomicNextPointer : util::Atomic<Coroutine *>
	{
	public:
		using util::Atomic<Coroutine *>::Atomic;

		/** Updates the reference to the next coroutine atomically.
		@param[in] next:
			The next coroutine.
		@return
			The previous value. */
		inline Coroutine * update(
			Coroutine * next);

		using util::Atomic<Coroutine *>::load_strong;

		/** Waits until the pointer points to another coroutine.
			Performs relaxed load_weak, until non-null value is read.
		@return
			The next waiting coroutine. */
		inline Coroutine * wait_weak();
		/** Waits until the pointer points to another coroutine.
			Performs a relaxed load_strong, and then `wait_weak()`, until non-null value is read.*/
		inline Coroutine * wait_strong();

		/** Releases the coroutine's state. */
		inline void release();
		/** Releases the coroutine's state and sets the next waiting coroutine. */
		inline void release_with_next(
			Coroutine * next);
		/** Acquires the coroutine's state using a strong load. */
		inline Coroutine * acquire_strong();
		/** Acquires the coroutine's state using a weak load. */
		inline Coroutine * acquire_weak();
	};

	/** A coroutine's reference to the next waiting coroutine. */
	union NextPointer
	{
		/** The single-threaded version. */
		Coroutine * plain;
		/** The thread-safe version. */
		AtomicNextPointer atomic;
	};
}

#include "NextPointer.inl"

#endif