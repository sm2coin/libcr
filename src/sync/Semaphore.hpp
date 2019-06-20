/** @file Semaphore.hpp
	Contains the semaphore type. */
#ifndef __libcr_sync_semaphore_hpp_defined
#define __libcr_sync_semaphore_hpp_defined

#include "ConditionVariable.hpp"

namespace cr::sync
{
	template<class ConditionVariable>
	/** POD semaphore type. */
	class PODSemaphorePattern
	{
		/** The semaphore's condition variable. */
		ConditionVariable m_cv;
		/** The semaphore's counter. */
		std::size_t m_counter;
	public:
		/** Initialises the semaphore.
		@param[in] counter:
			The semaphore's initial counter value. */
		void initialise(
			std::size_t counter);

		/** Helper class for waiting for a semaphore using `#CR_AWAIT`. */
		class WaitCall
		{
			/** The semaphore to wait for. */
			PODSemaphorePattern<ConditionVariable> &m_semaphore;
		public:
			/** Initialises the wait call.
			@param[in] semaphore:
				The semaphore to wait for. */
			constexpr WaitCall(
				PODSemaphorePattern<ConditionVariable> &semaphore);

			/** Waits for the semaphore.
			@param[in] coroutine:
				The coroutine to wait.
			@return
				Whether the operation blocks. */
			[[nodiscard]] mayblock libcr_wait(
				Coroutine * coroutine);
		};

		/** Waits for the semaphore.
			Blocks if the semaphore counter is 0. */
		[[nodiscard]] constexpr WaitCall wait();

		/** Notifies the semaphore.
		@return
			Whether any coroutine was directly notified. */
		bool notify();
	};

	template<class ConditionVariable>
	/** Semaphore type. */
	class SemaphorePattern : PODSemaphorePattern<ConditionVariable>
	{
	public:
		using PODSemaphorePattern<ConditionVariable>::wait;
		using PODSemaphorePattern<ConditionVariable>::notify;

		/** Creates a semaphore.
		@param[in] counter:
			The semaphore's initial counter value. */
		SemaphorePattern(
			std::size_t counter);
	};

	typedef PODSemaphorePattern<PODConditionVariable> PODSemaphore;
	typedef PODSemaphorePattern<PODFIFOConditionVariable> PODFIFOSemaphore;
	typedef SemaphorePattern<PODConditionVariable> Semaphore;
	typedef SemaphorePattern<PODFIFOConditionVariable> FIFOSemaphore;
}

#include "Semaphore.inl"

#endif