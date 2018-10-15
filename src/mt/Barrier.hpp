#ifndef __libcr_mt_barrier_defined
#define __libcr_mt_barrier_defined

#include <mutex>
#include "../sync/Barrier.hpp"

namespace cr::mt
{
	/** Multithreading enabled barrier type. */
	class Barrier : sync::Barrier
	{
		std::mutex m_mutex;
	public:
		/** Helper class for waiting for a barrier using `#CR_AWAIT`. */
		class WaitCall
		{
			/** The barrier to wait for. */
			Barrier &m_barrier;
		public:
			/** Creates a wait call.
			@param[in] barrier:
				The barrier to wait for. */
			constexpr WaitCall(
				Barrier &barrier);

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
}

#include "Barrier.inl"

#endif