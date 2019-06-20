/** @file Scheduler.hpp
	Contains a simple scheduler implementation. */
#ifndef __libcr_scheduler_hpp_defined
#define __libcr_scheduler_hpp_defined

#include "sync/Block.hpp"
#include "util/CVTraits.hpp"

namespace cr
{
	// Forward declarations.
	class Coroutine;

	template<class ConditionVariable>
	/** Simple scheduler using a single condition variable.
	@tparam ConditionVariable:
		The condition variable type. */
	class SchedulerPattern
	{
		static SchedulerPattern<ConditionVariable> s_instance;
		/** The scheduler's condition variable.
			This is used to notify waiting coroutines. */
		util::remove_cv_pod_t<ConditionVariable> m_cv;
	public:
		/** Returns a singleton instance. */
		static inline SchedulerPattern<ConditionVariable> &instance();

		/** Initialises the scheduler.
		@param[in] unused:
			Needed for compatibility with other scheduler types. */
		inline void initialise(
			std::size_t unused = 0);

		/** Progresses all currently waiting coroutines.
		@return
			Whether any coroutines were waiting. */
		bool schedule(
			std::size_t = 0);

		/** Enqueues a coroutine to wait for scheduling. */
		constexpr typename ConditionVariable::WaitCall enqueue();
	};
}

#include "Scheduler.inl"

#endif