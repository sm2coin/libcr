/** @file Barrier.hpp
	Contains a thread-safe barrier type. */
#ifndef __libcr_mt_barrier_defined
#define __libcr_mt_barrier_defined

#include "ConditionVariable.hpp"

namespace cr::mt
{
	template<class ConditionVariable>
	/** Multithreading enabled barrier type. */
	class PODBarrierPattern
	{
		/** The barrier's condition variable. */
		ConditionVariable m_cv;
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
			PODBarrierPattern<ConditionVariable> &m_barrier;
		public:
			/** Creates a wait call.
			@param[in] barrier:
				The barrier to wait for. */
			constexpr WaitCall(
				PODBarrierPattern<ConditionVariable> &barrier);

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

	template<class ConditionVariable>
	/** Multithreading enabled barrier type. */
	class BarrierPattern : PODBarrierPattern<ConditionVariable>
	{
	public:
		/** Initialises the barrier to an unblocking state. */
		BarrierPattern();
		/** Initialises the barrier to block `count` coroutines.
		@param[in] count:
			The amount of corouines to block. */
		explicit BarrierPattern(
			std::size_t count);

		using PODBarrierPattern<ConditionVariable>::set;
		using PODBarrierPattern<ConditionVariable>::wait;
	};

	/** Threadsafe POD barrier type without ordering guarantees. */
	typedef PODBarrierPattern<PODConditionVariable> PODBarrier;
	/** Threadsafe POD barrier type with FIFO ordering. */
	typedef PODBarrierPattern<PODFIFOConditionVariable> PODFIFOBarrier;
	/** Threadsafe barrier type without ordering guarantees. */
	typedef BarrierPattern<PODConditionVariable> Barrier;
	/** Threadsafe barrier type with FIFO ordering. */
	typedef BarrierPattern<PODFIFOConditionVariable> FIFOBarrier;
}

#include "Barrier.inl"

#endif