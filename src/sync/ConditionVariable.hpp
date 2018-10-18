/** @file ConditionVariable.hpp
	Contains the condition variable class. */
#ifndef __libcr_sync_conditionvariable_hpp_defined
#define __libcr_sync_conditionvariable_hpp_defined

#include "Block.hpp"

namespace cr
{
	class Coroutine;
}

namespace cr::sync
{
	/** POD condition variable with FIFO notifications. */
	class PODFIFOConditionVariable
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
		inline bool empty() const;

		/** Helper class for waiting for a condition variable using `#CR_AWAIT`. */
		class WaitCall
		{
			/** The condition variable to wait for. */
			PODFIFOConditionVariable &m_cv;
		public:
			/** Initialises the wait call.
			@param[in] cv:
				The condition variable to wait for. */
			constexpr WaitCall(
				PODFIFOConditionVariable &cv);

			/** Adds a coroutine to the queue.
			@param[in] coroutine:
				The coroutine to add to the waiting queue.
			@return
				Whether the call blocks. */
			block libcr_wait(
				Coroutine * coroutine);
		};

		/** Adds a coroutine to the queue. */
		[[nodiscard]] constexpr WaitCall wait();

		/** Returns the first coroutine in the waiting list. */
		inline Coroutine * front();

		/** Notifies the first waiting coroutine, if exists.
			Removes the notified coroutine from the waiting queue. */
		void notify_one();

		/** Notifies all waiting coroutines.
			Only notifies and removes coroutines that were waiting before the call, not those added during the call. */
		void notify_all();
	};

	/** Condition variable with FIFO notifications. */
	class FIFOConditionVariable : PODFIFOConditionVariable
	{
	public:
		/** Initialises the condition variable. */
		FIFOConditionVariable();

		using PODFIFOConditionVariable::empty;
		using PODFIFOConditionVariable::wait;
		using PODFIFOConditionVariable::front;
		using PODFIFOConditionVariable::notify_one;
		using PODFIFOConditionVariable::notify_all;
	};

	/** POD condition variable, with LIFO notifications. */
	class PODConditionVariable
	{
		/** The waiting coroutine.*/
		Coroutine * m_waiting;
	public:
		/** Initialises the object. */
		void initialise();

		/** Whether no coroutine is currently waiting. */
		inline bool empty() const;

		/** Helper class for waiting for a condition variable using `#CR_AWAIT`. */
		class WaitCall
		{
		protected:
			/** The condition variable to wait on. */
			PODConditionVariable &m_cv;
		public:
			/** Initialises the wait call.
			@param[in] cv:
				The condition variable to wait on. */
			constexpr WaitCall(
				PODConditionVariable &cv);

			/** Adds a coroutine to the queue.
			@param[in] coroutine:
				The coroutine to add to the waiting queue.
			@return
				Whether the call blocks. */
			block libcr_wait(
				Coroutine * coroutine);
		};

		/** Adds a coroutine to the queue. */
		[[nodiscard]] constexpr WaitCall wait();

		/** The waiting coroutine, or `null`. */
		inline Coroutine * front();

		/** Notifies the waiting coroutine, if exists. */
		void notify_one();

		/** Notifies all waiting coroutines. */
		void notify_all();
	};

	/** Condition variable with LIFO notifications. */
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
}

#include "ConditionVariable.inl"

#endif