#ifndef __libcr_mt_conditionvariable_hpp_defined
#define __libcr_mt_conditionvariable_hpp_defined

#include <mutex>
#include <atomic>
#include "../sync/ConditionVariable.hpp"

namespace cr
{
	class Coroutine;
}

namespace cr::mt
{
	/** Threadsafe condition variable. */
	class PODConditionVariable
	{
		/** The first waiting coroutine. */
		std::atomic<Coroutine *> m_first_waiting;
		/** The last waiting coroutine. */
		std::atomic<Coroutine *> m_last_waiting;
	public:
		/** Initialises the condition variable. */
		void initialise();

		/** Whether the condition variable's waiting queue is empty. */
		inline bool empty() const;

		/** Helper class for waiting for a condition variable using `#CR_AWAIT`. */
		class WaitCall
		{
			/** The condition variable to wait for. */
			PODConditionVariable &m_cv;
		public:
			/** Initialises the wait call.
			@param[in] cv:
				The condition variable to wait for. */
			constexpr WaitCall(
				PODConditionVariable &cv);

			/** Adds a coroutine to the queue.
			@param[in] coroutine:
				The coroutine to add to the waiting queue.
			@return
				Whether the call blocks. */
			[[nodiscard]] sync::block libcr_wait(
				Coroutine * coroutine);
		};

		/** Adds a coroutine to the queue. */
		[[nodiscard]] constexpr WaitCall wait();

		/** Notifies the first waiting coroutine, if exists.
			Removes the notified coroutine from the waiting queue. */
		void notify_one();

		/** Notifies all waiting coroutines.
			Only notifies and removes coroutines that were waiting before the call, not those added during the call. */
		void notify_all();
	};
}

#include "ConditionVariable.inl"

#endif