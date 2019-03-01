/** @file ConditionVariable.hpp
	Contains the condition variable class. */
#ifndef __libcr_sync_conditionvariable_hpp_defined
#define __libcr_sync_conditionvariable_hpp_defined

#include "Block.hpp"
#include "../Scheduler.hpp"

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
		bool notify_one();

		/** Notifies the first waiting coroutine, if exists, and sets its error flag.
			Removes the notified coroutine from the waiting queue.
		@return
			Whether a coroutine was notified. */
		bool fail_one();

		/** Notifies all waiting coroutines.
			Only notifies and removes coroutines that were waiting before the call, not those added during the call.
		@return
			Whether any coroutine was executed. */
		bool notify_all();

		/** Notifies all waiting coroutines, and sets their error flags.
			Only notifies and removes coroutines that were waiting before the call, not those added during the call.
		@return
			Whether a coroutine was notified. */
		bool fail_all();
	};

	/** Condition variable with FIFO notifications.
		In contrast to the POD version, this version calls `fail_all()` in the destructor. */
	class FIFOConditionVariable : public PODFIFOConditionVariable
	{
		using PODFIFOConditionVariable::initialise;
	public:
		/** Initialises the condition variable. */
		FIFOConditionVariable();
		/** Destroys the condition variable.
			Calls `fail_all()`. */
		~FIFOConditionVariable();
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

		/** Notifies the waiting coroutine, if exists.
		@return
			Whether a coroutine was notified. */
		bool notify_one();

		/** Notifies the first waiting coroutine, if exists, and sets its error flag.
			Removes the notified coroutine from the waiting queue.
		@return
			Whether a coroutine was notified. */
		bool fail_one();

		/** Notifies all waiting coroutines.
		@return
			Whether any coroutine was executed. */
		bool notify_all();

		/** Notifies all waiting coroutines, and sets their error flags.
			Only notifies and removes coroutines that were waiting before the call, not those added during the call.
		@return
			Whether a coroutine was notified. */
		bool fail_all();
	};

	/** Condition variable with LIFO notifications.
		 In contrast to the POD version, this version calls `fail_all()` in the destructor. */
	class ConditionVariable : public PODConditionVariable
	{
		using PODConditionVariable::initialise;
	public:
		/** Initialises the condition variable. */
		ConditionVariable();
		/** Destroys the condition variable.
			Calls `fail_all()`. */
		~ConditionVariable();
	};

	typedef cr::SchedulerBase<FIFOConditionVariable> FIFOScheduler;
	typedef cr::SchedulerBase<ConditionVariable> Scheduler;
}

#include "ConditionVariable.inl"

#endif