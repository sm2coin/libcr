/** @file ConditionVariable.hpp
	Contains the condition variable class. */
#ifndef __libcr_sync_conditionvariable_hpp_defined
#define __libcr_sync_conditionvariable_hpp_defined

#include "Block.hpp"


#ifdef LIBCR_INLINE
#define __LIBCR_INLINE inline
#else
#define __LIBCR_INLINE
#endif


namespace cr
{
	class Coroutine;
	template<class ConditionVariable>
	class SchedulerPattern;
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
		__LIBCR_INLINE void initialise();

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
			__LIBCR_INLINE block libcr_wait(
				Coroutine * coroutine);
		};

		/** Adds a coroutine to the queue. */
		[[nodiscard]] constexpr WaitCall wait();

		/** Returns the first coroutine in the waiting list. */
		inline Coroutine * front();

		/** Notifies the first waiting coroutine, if exists.
			Removes the notified coroutine from the waiting queue.
		@return
			Whether a coroutine was removed. */
		__LIBCR_INLINE bool notify_one();

		/** Notifies the first waiting coroutine, if exists, and sets its error flag.
			Removes the notified coroutine from the waiting queue.
		@return
			Whether a coroutine was notified. */
		__LIBCR_INLINE bool fail_one();

		/** Removes the first waiting coroutine, if exists.
			Does not notify the removed coroutine.
		@return
			The removed coroutine, or null. */
		__LIBCR_INLINE Coroutine * remove_one();

		/** Notifies all waiting coroutines.
			Only notifies and removes coroutines that were waiting before the call, not those added during the call.
		@return
			Whether any coroutine was executed. */
		__LIBCR_INLINE bool notify_all();

		/** Notifies all waiting coroutines, and sets their error flags.
			Only notifies and removes coroutines that were waiting before the call, not those added during the call.
		@return
			Whether a coroutine was notified. */
		__LIBCR_INLINE bool fail_all();

		/** Removes all waiting coroutines.
			Does not notify the removed coroutines.
		@return
			The first removed coroutine, or null. */
		__LIBCR_INLINE Coroutine * remove_all();
	};

	/** Condition variable with FIFO notifications.
		In contrast to the POD version, this version calls `fail_all()` in the destructor. */
	class FIFOConditionVariable : public PODFIFOConditionVariable
	{
		using PODFIFOConditionVariable::initialise;
	public:
		/** Initialises the condition variable. */
		__LIBCR_INLINE FIFOConditionVariable();
		/** Destroys the condition variable.
			Calls `fail_all()`. */
		__LIBCR_INLINE ~FIFOConditionVariable();
	};

	/** POD condition variable, with LIFO notifications. */
	class PODConditionVariable
	{
		/** The waiting coroutine.*/
		Coroutine * m_waiting;
	public:
		/** Initialises the object. */
		__LIBCR_INLINE void initialise();

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
			__LIBCR_INLINE block libcr_wait(
				Coroutine * coroutine);
		};

		/** Adds a coroutine to the queue. */
		[[nodiscard]] constexpr WaitCall wait();

		/** The waiting coroutine, or `null`. */
		inline Coroutine * front();

		/** Notifies the waiting coroutine, if exists.
		@return
			Whether a coroutine was notified. */
		__LIBCR_INLINE bool notify_one();

		/** Notifies the first waiting coroutine, if exists, and sets its error flag.
			Removes the notified coroutine from the waiting queue.
		@return
			Whether a coroutine was notified. */
		__LIBCR_INLINE bool fail_one();

		/** Removes the first waiting coroutine, if exists.
			Does not notify the removed coroutine.
		@return
			The removed coroutine, or null. */
		__LIBCR_INLINE Coroutine * remove_one();

		/** Notifies all waiting coroutines.
		@return
			Whether any coroutine was executed. */
		__LIBCR_INLINE bool notify_all();

		/** Notifies all waiting coroutines, and sets their error flags.
			Only notifies and removes coroutines that were waiting before the call, not those added during the call.
		@return
			Whether a coroutine was notified. */
		__LIBCR_INLINE bool fail_all();

		/** Removes all waiting coroutines.
			Does not notify the removed coroutines.
		@return
			The first removed coroutine, or null. */
		__LIBCR_INLINE Coroutine * remove_all();
	};

	/** Condition variable with LIFO notifications.
		 In contrast to the POD version, this version calls `fail_all()` in the destructor. */
	class ConditionVariable : public PODConditionVariable
	{
		using PODConditionVariable::initialise;
	public:
		/** Initialises the condition variable. */
		__LIBCR_INLINE ConditionVariable();
		/** Destroys the condition variable.
			Calls `fail_all()`. */
		__LIBCR_INLINE ~ConditionVariable();
	};

	typedef cr::SchedulerPattern<FIFOConditionVariable> FIFOScheduler;
	typedef cr::SchedulerPattern<ConditionVariable> Scheduler;
}

#include "ConditionVariable.inl"

#endif