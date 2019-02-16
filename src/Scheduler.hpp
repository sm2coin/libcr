#ifndef __libcr_scheduler_hpp_defined
#define __libcr_scheduler_hpp_defined

#include "sync/Block.hpp"

namespace cr
{
	// Forward declarations.
	class Coroutine;

	template<class ConditionVariable>
	/** Simple scheduler using a condition variable.
	@tparam PODConditionVariable:
		The POD condition variable type. */
	class SchedulerBase
	{
		/** The scheduler's condition variable.
			This is used to notify waiting coroutines. */
		ConditionVariable m_cv;
	public:
		/** Returns a singleton instance. */
		static inline SchedulerBase<ConditionVariable> &instance();

		/** Progresses all currently waiting coroutines.
		@return
			Whether any coroutines were waiting. */
		bool schedule();

		/** Enqueues a coroutine to wait */
		constexpr typename ConditionVariable::WaitCall enqueue();
	};
}

#include "Scheduler.inl"

#endif