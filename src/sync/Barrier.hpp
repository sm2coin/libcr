/** @file Barrier.hpp
	Contains the barrier class. */
#ifndef __libcr_sync_barrier_hpp_defined
#define __libcr_sync_barrier_hpp_defined

#include "ConditionVariable.hpp"
#include "Block.hpp"

#include <cstddef>

namespace cr::sync
{
	template<class ConditionVariable>
	/** POD barrier type. */
	class PODBarrierBase
	{
		/** The barrier's condition variable, to block coroutines. */
		ConditionVariable m_cv;
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

		/** Sets the barrier to block the requested amount of coroutines.
		@param[in] count:
			How many coroutines to block.*/
		void set(
			std::size_t count);

		/** Helper class for waiting for a barrier using `#CR_AWAIT`. */
		class WaitCall
		{
			/** The barrier to wait for. */
			PODBarrierBase<ConditionVariable> &m_barrier;
		public:
			/** Initialises the wait call.
			@param[in] barrier:
				The barrier to wait for. */
			constexpr WaitCall(
				PODBarrierBase<ConditionVariable> &barrier);

			/** Waits for the barrier.
			@param[in] coroutine:
				The coroutine to wait.
			@return
				Whether the operation blocks. */
			[[nodiscard]] mayblock libcr_wait(
				Coroutine * coroutine);
		};

		/** Waits for the barrier.
			Blocks all coroutines until the configured amount of coroutines are waiting. To be used with `#CR_AWAIT`. */
		[[nodiscard]] constexpr WaitCall wait();
	};

	template<class ConditionVariable>
	/** Barrier type. */
	class BarrierBase : PODBarrierBase<ConditionVariable>
	{
	public:
		/** Initialises the barrier to an unblocking state. */
		BarrierBase();
		/** Initialises the barrier to block `count` coroutines.
		@param[in] count:
			The amount of coroutines to block. */
		explicit BarrierBase(
			std::size_t count);

		using PODBarrierBase<ConditionVariable>::set;
		using PODBarrierBase<ConditionVariable>::wait;
	};

	typedef PODBarrierBase<PODConditionVariable> PODBarrier;
	typedef PODBarrierBase<PODFIFOConditionVariable> PODFIFOBarrier;
	typedef BarrierBase<PODConditionVariable> Barrier;
	typedef BarrierBase<PODFIFOConditionVariable> FIFOBarrier;
}

#include "Barrier.inl"

#endif