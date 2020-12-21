/** @file HybridScheduler.hpp
	Contains a multi-threaded scheduler that allows load balancing and keeps thread affinity. */
#ifndef __libcr_hybridscheduler_hpp_defined
#define __libcr_hybridscheduler_hpp_defined

#include "detail/Thread.hpp"
#include "util/Atomic.hpp"
#include "sync/Block.hpp"
#include "util/Rng.hpp"

#include <vector>

namespace cr
{
	template<class MtCV, class SyncCV>
	/** Multi-threaded scheduler type that allows load balancing and keeps thread affinity.
	@tparam MtCV:
		The multi-threading enabled condition variable type to use when migrating coroutines between threads.
	@tparam SyncCV:
		The thread-unsafe condition variable type to use when a coroutine stays within its thread. */
	class HybridScheduler
	{
		/** The static scheduler instance. */
		static HybridScheduler<MtCV, SyncCV> s_instance;

		typedef std::uint64_t time_t;
		/** Thread context type. */
		struct ThreadContext
		{
			/** Initialises the thread context. */
			ThreadContext();
			/** The coroutines added by other threads. */
			MtCV global_cv;
			/** The coroutines owned by the thread. */
			SyncCV local_cv;
			/** The time needed to execute all coroutines once. */
			util::Atomic<time_t> load;
			/** The thread's scheduling RNG. */
			util::Rng rng;
		};

		/** The scheduler's thread contexts. */
		std::vector<ThreadContext> m_threads;
		/** The thread with the highest load. */
		util::Atomic<std::size_t> m_busy_thread;
		/** The thread with the lowest load. */
		util::Atomic<std::size_t> m_idle_thread;


		/** Detects the current load distribution of the scheduler threads. */
		inline void detect_load();

	public:
		/** Initialises the scheduler. */
		HybridScheduler();

		/** Initialises the scheduler to support a specified number of threads.
		@param[in] threads:
			The number of threads to support.
			Must match the number of actual scheduling threads. */
		void initialise(
			std::size_t threads);

		/** The number of threads run by the scheduler. */
		inline std::size_t threads() const;

		/** Executes all coroutines currently waiting for a specified thread.
			Records the thread's execution time and auto-balances the load.
		@param[in] thread:
			The thread index.
		@return
			Whether any coroutines were executed. */
		inline bool schedule(
			std::size_t thread = 0);

		/** Helper class for enqueuing a coroutine into the scheduler using `#CR_AWAIT`.  */
		class EnqueueCall
		{
			/** The scheduler to enqueue into.*/
			HybridScheduler<MtCV, SyncCV> &m_scheduler;
		public:
			/** Initialises the enqueue call.
			@param[in] scheduler:
				The scheduler to enqueue into. */
			constexpr EnqueueCall(
				HybridScheduler<MtCV, SyncCV> * scheduler);

			/** Enqueues a coroutine in the scheduler.
			@param[in] coroutine:
				The coroutine to enqueue. */
			[[nodiscard]] sync::block libcr_wait(
				Coroutine * coroutine);
		};

		/** Enqueues a coroutine in the scheduler. */
		[[nodiscard]] constexpr EnqueueCall enqueue();

		/** Retrieves the static scheduler instance. */
		static inline HybridScheduler<MtCV, SyncCV> &instance();
	};
}

#include "HybridScheduler.inl"

#endif