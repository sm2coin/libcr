#ifndef __libcr_mt_conditionvariable_hpp_defined
#define __libcr_mt_conditionvariable_hpp_defined

#include "../sync/Block.hpp"
#include "../Scheduler.hpp"

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
			Removes the notified coroutine from the waiting queue.
		@return
			Whether a coroutine was notified. */
		bool notify_one();

		/** Removes the first waiting coroutine, if exists.
			Removes the first coroutine from the waiting queue.
			`resume_and_wait_for_completion()` needs to be called on the removed coroutine before accessing it.
			The coroutine is not resumed.
		@return
			The removed coroutine, or null. */
		Coroutine * remove_one();

		/** Notifies all waiting coroutines.
			Only notifies and removes coroutines that were waiting before the call, not those added during the call.
		@return
			Whether any coroutines were notified. */
		bool notify_all();

		/** Removes the all waiting coroutines from the waiting queue.
			`resume_and_wait_for_completion()` needs to be called on all removed coroutines before accessing them.
			No coroutine is resumed.
		@param[out] first:
			The first removed coroutine.
		@param[out] last:
			The last removed coroutine.
		@return
			Whether any coroutines were removed. */
		bool remove_all(
			Coroutine * &first,
			Coroutine * &last);

		/** Resumes a removed coroutine and waits until it is safe to access.
			This does not execute a removed coroutine.
		@param[in] coroutine:
			The coroutine to resume.
		@param[in] last:
			The last removed coroutine.
		@return
			The next waiting coroutine, or null. */
		static Coroutine * resume_and_wait_for_completion(
			Coroutine * coroutine,
			Coroutine * last);
	};

	/** Threadsafe condition variable with FIFO notifications. */
	class FIFOConditionVariable : PODFIFOConditionVariable
	{
	public:
		/** Initialises the condition variable. */
		FIFOConditionVariable();

		using PODFIFOConditionVariable::WaitCall;

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
		@return
			Whether a coroutine was notified. */
		bool notify_one();

		/** Removes the first waiting coroutine, if exists.
			Removes the first coroutine from the waiting queue.
			`resume_and_wait_for_completion()` needs to be called on the removed coroutine before accessing it.
			The coroutine is not resumed.
		@return
			The removed coroutine, or null. */
		Coroutine * remove_one();

		/** Notifies all waiting coroutines.
			Only notifies and removes coroutines that were waiting before the call, not those added during the call.
		@return
			Whether any coroutines were notified. */
		bool notify_all();

		/** Removes the all waiting coroutines from the waiting queue.
			`resume_and_wait_for_completion()` needs to be called on all removed coroutines before accessing them.
			No coroutine is resumed.
		@param[out] first:
			The first removed coroutine.
		@param[out] last:
			Ignored. Used for compatibility with FIFO CVs.
		@return
			Whether any coroutines were removed. */
		bool remove_all(
			Coroutine * &first,
			Coroutine * &last);

		/** Resumes a removed coroutine.
			This does not execute a removed coroutine.
		@param[in] coroutine:
			The coroutine to resume.
		@param[in] last:
			The last removed coroutine.
		@return
			The next coroutine, or null. */
		static Coroutine * resume_and_wait_for_completion(
			Coroutine * coroutine,
			Coroutine * last);
	};

	/** Threadsafe condition variable without notification ordering guarantees.
		As long as only `notify_all()` is used, and not `notify_one()`, the coroutines are guaranteed to be notified in LIFO order. `notify_one()` can reorder the queue under certain circumstances. */
	class ConditionVariable : PODConditionVariable
	{
	public:
		/** Initialises the condition variable. */
		ConditionVariable();

		using PODConditionVariable::WaitCall;

		using PODConditionVariable::empty;
		using PODConditionVariable::wait;
		using PODConditionVariable::notify_one;
		using PODConditionVariable::notify_all;
	};

	typedef cr::SchedulerBase<FIFOConditionVariable> FIFOScheduler;
	typedef cr::SchedulerBase<ConditionVariable> Scheduler;
}

#include "ConditionVariable.inl"

#endif