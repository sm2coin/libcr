#ifndef __libcr_mt_barrier_defined
#define __libcr_mt_barrier_defined

#include "ConditionVariable.hpp"

namespace cr::mt
{
	/** Multithreading enabled barrier type. */
	class PODBarrier
	{
		/** The barrier's condition variable. */
		PODConditionVariable m_cv;
		/** How many coroutines to block. */
		std::atomic_size_t m_count;
	public:
		/** Initialises the barrier to an unblocked state. */
		void initialise();
		/** Initialises the barrier to block `count` coroutines.
		@param[in] count:
			How many coroutines to block. */
		void initialise(
			std::size_t count);

		/** Sets the barrier to block the requested amount of coroutines.
		@param[in] count:
			How many coroutines to block. */
		void set(
			std::size_t count);

		/** Helper class for waiting for a barrier using `#CR_AWAIT`. */
		class WaitCall
		{
			/** The barrier to wait for. */
			PODBarrier &m_barrier;
		public:
			/** Creates a wait call.
			@param[in] barrier:
				The barrier to wait for. */
			constexpr WaitCall(
				PODBarrier &barrier);

			/** Waits for the barrier.
			@param[in] coroutine:
				The coroutine to wait.
			@return
				Whether the operation blocks. */
			[[nodiscard]] sync::mayblock libcr_wait(
				Coroutine * coroutine);
		};

		/** Waits for the barrier.
			Blocks all coroutines until the configured amount of coroutines are waiting. To be used with `#CR_AWAIT`. */
		[[nodiscard]] constexpr WaitCall wait();
	};

	/** Multithreading enabled barrier type. */
	class Barrier : PODBarrier
	{
	public:
		/** Initialises the barrier to an unblocking state. */
		Barrier();
		/** Initialises the barrier to block `count` coroutines.
		@param[in] count:
			The amount of corouines to block. */
		explicit Barrier(
			std::size_t count);

		using PODBarrier::set;
		using PODBarrier::wait;
	};
}

#include "Barrier.inl"

#endif