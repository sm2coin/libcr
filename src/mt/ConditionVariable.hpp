/** @file ConditionVariable.hpp
	Contains the thread-safe condition variables. */
#ifndef __libcr_mt_conditionvariable_hpp_defined
#define __libcr_mt_conditionvariable_hpp_defined

#include "../sync/Block.hpp"

#include <atomic>

namespace cr
{
	class Coroutine;
	template<class ConditionVariable>
	class SchedulerPattern;
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
			/** Whether to invalidate the waiting coroutine's thread. */
			bool m_invalidate_thread;
		public:
			/** Initialises the wait call.
			@param[in] cv:
				The condition variable to wait for.
			@param[in] invalidate_thread:
				Whether to invalidate the waiting coroutine's thread. */
			constexpr WaitCall(
				PODFIFOConditionVariable &cv,
				bool invalidate_thread);

			/** Adds a coroutine to the queue.
			@param[in] coroutine:
				The coroutine to add to the waiting queue.
			@return
				Whether the call blocks. */
			[[nodiscard]] sync::block libcr_wait(
				Coroutine * coroutine) { return libcr_wait(coroutine, coroutine); }
			[[nodiscard]] sync::block libcr_wait(
				Coroutine * coroutine,
				Coroutine * last);
		};

		/** Adds a coroutine to the queue.
		@param[in] invalidate_thread:
			Whether to invalidate the waiting coroutine's thread. */
		[[nodiscard]] constexpr WaitCall wait(
			bool invalidate_thread = true);

		/** Notifies the first waiting coroutine, if exists.
			Removes the notified coroutine from the waiting queue.
		@return
			Whether a coroutine was notified. */
		bool notify_one();

		/** Notifies the first waiting coroutine, if exists, and sets its error flag.
			Removes the notified coroutine from the waiting queue.
		@return
			Whether a coroutine was notified. */
		bool fail_one();

		/** Removes the first waiting coroutine, if exists.
			Removes the first coroutine from the waiting queue. The coroutine is acquired automatically, but not executed.
		@return
			The removed coroutine, or null. */
		Coroutine * remove_one();

		/** Notifies all waiting coroutines.
			Only notifies and removes coroutines that were waiting before the call, not those added during the call.
		@return
			Whether any coroutines were notified. */
		bool notify_all();

		/** Notifies all waiting coroutines, and sets their error flags.
			Only notifies and removes coroutines that were waiting before the call, not those added during the call.
		@return
			Whether a coroutine was notified. */
		bool fail_all();

		/** Removes the all waiting coroutines from the waiting queue.
			`acquire_and_complete()` needs to be called on all removed coroutines before accessing them. No coroutine is acquired or executed.
		@param[out] first:
			The first removed coroutine.
		@param[out] last:
			The last removed coroutine.
		@return
			Whether any coroutines were removed. */
		bool remove_all(
			Coroutine * &first,
			Coroutine * &last);

		/** Acquires a removed coroutine and waits until it is safe to access.
			This does not execute a removed coroutine. Only necessary in combination with `remove_all()`.
		@param[in] coroutine:
			The coroutine to acquire.
		@param[in] last:
			The last removed coroutine.
		@return
			The next waiting coroutine, or null. */
		static Coroutine * acquire_and_complete(
			Coroutine * coroutine,
			Coroutine * last);
	};

	/** Threadsafe condition variable with FIFO notifications.
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
			/** Whether to invalidate the waiting coroutine's thread. */
			bool m_invalidate_thread;
		public:
			/** Initialises the wait call.
			@param[in] cv:
				The condition variable to wait for.
			@param[in] invalidate_thread:
				Whether to invalidate the waiting coroutine's thread. */
			constexpr WaitCall(
				PODConditionVariable &cv,
				bool invalidate_thread);

			/** Adds a coroutine to the queue.
				The coroutine added to the queue is the first to be notified (LIFO).
			@param[in] coroutine:
				The coroutine to add to the waiting queue.
			@return
				Whether the call blocks. */
			[[nodiscard]] sync::block libcr_wait(
				Coroutine * coroutine) { return libcr_wait(coroutine, coroutine); }
			[[nodiscard]] sync::block libcr_wait(
				Coroutine * coroutine,
				Coroutine * last);
		};

		/** Adds a coroutine to the queue.
		@param[in] invalidate_thread:
			Whether to invalidate the waiting coroutine's thread. */
		[[nodiscard]] constexpr WaitCall wait(
			bool invalidate_thread = true);

		/** Notifies the first waiting coroutine, if exists.
			Removes the notified coroutine from the waiting queue.
		@return
			Whether a coroutine was notified. */
		bool notify_one();

		/** Notifies the first waiting coroutine, if exists, and sets its error flag.
			Removes the notified coroutine from the waiting queue.
		@return
			Whether a coroutine was notified. */
		bool fail_one();

		/** Removes the first waiting coroutine, if exists.
			Removes the first coroutine from the waiting queue. The coroutine is acquired automatically, but not executed.
		@return
			The removed coroutine, or null. */
		Coroutine * remove_one();

		/** Notifies all waiting coroutines.
			Only notifies and removes coroutines that were waiting before the call, not those added during the call.
		@return
			Whether any coroutines were notified. */
		bool notify_all();

		/** Notifies all waiting coroutines, and sets their error flags.
			Only notifies and removes coroutines that were waiting before the call, not those added during the call.
		@return
			Whether a coroutine was notified. */
		bool fail_all();

		/** Removes the all waiting coroutines from the waiting queue.
			`resume_and_wait_for_completion()` needs to be called on all removed coroutines before accessing them. No coroutine is acquired or executed.
		@param[out] first:
			The first removed coroutine.
		@param[out] last:
			Ignored. Used for compatibility with FIFO CVs.
		@return
			Whether any coroutines were removed. */
		bool remove_all(
			Coroutine * &first,
			Coroutine * &last);

		/** Acquires a removed coroutine and waits until it is safe to access.
			This does not execute a removed coroutine.
		@param[in] coroutine:
			The coroutine to resume.
		@param[in] ignored:
			Dummy parameter, for compatibility with the FIFO version.
		@return
			The next coroutine, or null. */
		static Coroutine * acquire_and_complete(
			Coroutine * coroutine,
			Coroutine * ignored);
	};

	/** Threadsafe condition variable without notification ordering guarantees.
		As long as only `notify_all()` is used, and not `notify_one()`, the coroutines are guaranteed to be notified in LIFO order. `notify_one()` can reorder the queue under certain circumstances. In contrast to the POD version, this version calls `fail_all()` in the destructor. */
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

	typedef cr::SchedulerPattern<FIFOConditionVariable> FIFOScheduler;
	typedef cr::SchedulerPattern<ConditionVariable> Scheduler;
}

#include "ConditionVariable.inl"

#endif