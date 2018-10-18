#ifndef __libcr_mt_conditionvariable_hpp_defined
#define __libcr_mt_conditionvariable_hpp_defined

#include "../sync/Block.hpp"

#include <mutex>
#include <atomic>

namespace cr
{
	class Coroutine;
}

namespace cr::mt
{
	/** Threadsafe POD condition variable with FIFO notifications. */
	class PODFIFOConditionVariable
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

	/** Threadsafe condition variable with FIFO notifications. */
	class FIFOConditionVariable : PODFIFOConditionVariable
	{
	public:
		/** Initialises the condition variable. */
		FIFOConditionVariable();

		using PODFIFOConditionVariable::empty;
		using PODFIFOConditionVariable::wait;
		using PODFIFOConditionVariable::notify_one;
		using PODFIFOConditionVariable::notify_all;
	};

	/** Threadsafe POD condition variable without notification ordering guarantees.
		As long as only `notify_all()` is used, and not `notify_one()`, the coroutines are guaranteed to be notified in LIFO order. `notify_one()` can reorder the queue under certain circumstances. */
	class PODConditionVariable
	{
		/** The latest waiting coroutine. */
		std::atomic<Coroutine *> m_waiting;
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
				The coroutine added to the queue is the first to be notified (LIFO).
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
			Removes the notified coroutine from the waiting queue.
			Might reorder the coroutine queue. */
		void notify_one();

		/** Notifies all waiting coroutines.
			Only notifies and removes coroutines that were waiting before the call, not those added during the call.
			Notifies all coroutines in the reverse error. */
		void notify_all();
	};

	/** Threadsafe condition variable without notification ordering guarantees.
		As long as only `notify_all()` is used, and not `notify_one()`, the coroutines are guaranteed to be notified in LIFO order. `notify_one()` can reorder the queue under certain circumstances. */
	class ConditionVariable : PODConditionVariable
	{
	public:
		/** Initialises the condition variable. */
		ConditionVariable();

		using PODConditionVariable::empty;
		using PODConditionVariable::wait;
		using PODConditionVariable::notify_one;
		using PODConditionVariable::notify_all;
	};
}

#include "ConditionVariable.inl"

#endif