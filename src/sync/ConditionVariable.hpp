/** @file ConditionVariable.hpp
	Contains the condition variable class. */
#ifndef __libcr_sync_conditionvariable_hpp_defined
#define __libcr_sync_conditionvariable_hpp_defined

namespace cr
{
	class Coroutine;
}

namespace cr::sync
{
	/** POD condition variable that can be waited on by an arbitrary number of coroutines. */
	class PODConditionVariable
	{
		/** The first coroutine waiting. */
		Coroutine * m_first_waiting;
		/** The last coroutine waiting.
			Needed for O(1) enqueuing. */
		Coroutine * m_last_waiting;
	public:
		/** Initialises the waiting queue. */
		void initialise();

		/** Whether the waiting list is empty. */
		inline bool empty();

		/** Adds a coroutine to the queue.
		@param[in] coroutine:
			The coroutine to add to the waiting queue. */
		void wait(
			Coroutine * coroutine);

		/** Returns the first coroutine in the waiting list. */
		inline Coroutine * front();

		/** Notifies the first waiting coroutine, if exists.
			Removes the notified coroutine from the waiting queue. */
		void notify_one();

		/** Notifies all waiting coroutines.
			Only notifies and removes coroutines that were waiting before the call, not those added during the call. */
		void notify_all();
	};

	/** Condition variable that can be waited on by an arbitrary number of coroutines. */
	class ConditionVariable : PODConditionVariable
	{
	public:
		/** Initialises the condition variable. */
		ConditionVariable();

		using PODConditionVariable::empty;
		using PODConditionVariable::wait;
		using PODConditionVariable::front;
		using PODConditionVariable::notify_one;
		using PODConditionVariable::notify_all;
	};

	/** POD condition variable that can be waited on by a single coroutine. */
	class PODSingleConditionVariable
	{
		/** The waiting coroutine.*/
		Coroutine * m_waiting;
	public:
		/** Initialises the object. */
		void initialise();

		/** Whether no coroutine is currently waiting. */
		inline bool empty();

		/** Enqueues a coroutine for waiting.
			There must not be a coroutine waiting on this condition variable already.
		@param[in] coroutine:
			The coroutine to enqueue. */
		void wait(
			Coroutine * coroutine);

		/** The waiting coroutine, or `null`. */
		Coroutine * front();

		/** Notifies the waiting coroutine, if exists. */
		void notify_one();

		/** Notifies all waiting coroutines. */
		void notify_all();
	};

	/** Condition variable that can be waited on by a single coroutine. */
	class SingleConditionVariable : PODSingleConditionVariable
	{
	public:
		/** Initialises the condition variable. */
		SingleConditionVariable();

		using PODSingleConditionVariable::empty;
		using PODSingleConditionVariable::wait;
		using PODSingleConditionVariable::front;
		using PODSingleConditionVariable::notify_one;
		using PODSingleConditionVariable::notify_all;
	};
}

#include "ConditionVariable.inl"

#endif