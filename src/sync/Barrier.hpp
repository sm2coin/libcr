/** @file Barrier.hpp
	Contains the barrier class. */
#ifndef __libcr_sync_barrier_hpp_defined
#define __libcr_sync_barrier_hpp_defined

#include "ConditionVariable.hpp"

#include <cstddef>

namespace cr::sync
{
	/** POD barrier type. */
	class PODBarrier
	{
		/** The barrier's condition variable, to block coroutines. */
		PODConditionVariable m_cv;
		/** How many coroutines still need to wait until the barrier unblocks. */
		std::size_t m_count;
	public:
		/** Initialises the barrier to an unblocking state. */
		void initialise();
		/** Initialises the barrier to block `count` coroutines.
		@param[in] count:
			The amount of coroutines to block. */
		void initialise(
			std::size_t count);

		/** Waits for the barrier.
			Blocks all coroutines until the configured amount of coroutines are waiting. To be used with `#CR_AWAIT`.
		@param[in] coroutine:
			The coroutine waiting for the barrier. */
		bool wait(
			Coroutine * coroutine);
	};

	/** Barrier type. */
	class Barrier : PODBarrier
	{
	public:
		/** Initialises the barrier to an unblocking state. */
		Barrier();
		/** Initialises the barrier to block `count` coroutines.
		@param[in] count:
			The amount of coroutines to block. */
		Barrier(
			std::size_t count);

		using PODBarrier::wait;
	};
}

#endif