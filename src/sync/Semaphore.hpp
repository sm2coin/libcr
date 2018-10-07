#ifndef __libcr_sync_semaphore_hpp_defined
#define __libcr_sync_semaphore_hpp_defined

#include "ConditionVariable.hpp"

namespace cr::sync
{
	/** POD semaphore type. */
	class PODSemaphore
	{
		/** The semaphore's condition variable. */
		PODConditionVariable m_cv;
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
			PODSemaphore &m_semaphore;
		public:
			/** Initialises the wait call.
			@param[in] semaphore:
				The semaphore to wait for. */
			constexpr WaitCall(
				PODSemaphore &semaphore);

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

		/** Notifies the semaphore. */
		void notify();
	};

	/** Semaphore type. */
	class Semaphore : PODSemaphore
	{
	public:
		using PODSemaphore::wait;
		using PODSemaphore::notify;

		/** Creates a semaphore.
		@param[in] counter:
			The semaphore's initial counter value. */
		Semaphore(
			std::size_t counter);
	};
}

#include "Semaphore.inl"

#endif