#include <timer/Timer.hpp>
#include <thread>

namespace cr
{
	template<class MtCV, class SyncCV>
	HybridScheduler<MtCV, SyncCV> HybridScheduler<MtCV, SyncCV>::s_instance;

	template<class MtCV, class SyncCV>
	HybridScheduler<MtCV, SyncCV>::ThreadContext::ThreadContext():
		global_cv(),
		local_cv(),
		load(~(time_t)0)
	{
	}

	template<class MtCV, class SyncCV>
	void HybridScheduler<MtCV, SyncCV>::initialise(
		std::size_t threads)
	{
		if(threads < 2)
			threads = 2;
		m_busy_thread.store(0, std::memory_order_relaxed);
		m_idle_thread.store(0, std::memory_order_relaxed);
		m_threads.~vector();
		new (&m_threads) std::vector<ThreadContext>{threads};
	}

	template<class MtCV, class SyncCV>
	void HybridScheduler<MtCV, SyncCV>::detect_load()
	{
		std::size_t idle = 0;
		time_t idle_time = m_threads[0].load.load_weak(std::memory_order_relaxed);
		std::size_t busy = 0;
		time_t busy_time = idle_time;

		for(std::size_t i = 1; i < m_threads.size(); i++)
		{
			time_t time = m_threads[i].load.load_weak(std::memory_order_relaxed);
			if(time < idle_time)
			{
				idle_time = time;
				idle = i;
			} else if(time > busy_time)
			{
				busy_time = time;
				busy = i;
			}
		}

		m_busy_thread.store(busy, std::memory_order_relaxed);
		m_idle_thread.store(idle, std::memory_order_relaxed);
	}

	template<class MtCV, class SyncCV>
	HybridScheduler<MtCV, SyncCV>::HybridScheduler():
		m_threads(std::thread::hardware_concurrency()),
		m_busy_thread(0),
		m_idle_thread(0)
	{
	}

	template<class MtCV, class SyncCV>
	bool HybridScheduler<MtCV, SyncCV>::schedule(
		std::size_t thread)
	{
		std::size_t busy = m_busy_thread.load_weak(std::memory_order_relaxed);
		bool balance = busy == thread;

		ThreadContext &ctx = m_threads[thread];
		Coroutine * gfirst, * glast;
		bool result = ctx.global_cv.remove_all(gfirst, glast);

		std::uint8_t counter = 0;
		// Every 64th coroutine is balanced.
		static constexpr std::uint8_t k_balance_mask = 0x3f;

		timer::Timer<std::chrono::microseconds> timer;
		Coroutine * lfirst;
		if((lfirst = ctx.local_cv.remove_all()))
			result = true;

		Coroutine * next;

		timer.start();

		while(lfirst)
		{
			next = lfirst->libcr_next_waiting.plain;
			if(balance
			&& (counter++ & k_balance_mask) == 0)
				lfirst->libcr_thread = detail::Thread::kInvalid;

			(*lfirst)();
			lfirst = next;
		}

		while(gfirst)
		{
			next = MtCV::acquire_and_complete(gfirst, glast);
			if(balance)
				gfirst->libcr_thread = detail::Thread::kInvalid;

			(*gfirst)();
			gfirst = next;
		}

		// The first thread updates the statistics.
		if(thread == 0)
			detect_load();

		ctx.load.store(timer.stop(), std::memory_order_relaxed);

		return result;
	}

	template<class MtCV, class SyncCV>
	constexpr HybridScheduler<MtCV, SyncCV>::EnqueueCall::EnqueueCall(
		HybridScheduler<MtCV, SyncCV> * scheduler):
		m_scheduler(*scheduler)
	{
	}

	template<class MtCV, class SyncCV>
	sync::block HybridScheduler<MtCV, SyncCV>::EnqueueCall::libcr_wait(
		Coroutine * coroutine)
	{
		if(!detail::valid(coroutine->libcr_thread))
		{
			std::size_t idle_thread = m_scheduler.m_idle_thread.load_weak(std::memory_order_relaxed);
			coroutine->libcr_thread = (detail::Thread) idle_thread;
			return m_scheduler.m_threads[idle_thread].global_cv.wait(false).libcr_wait(coroutine);
		} else
		{
			return m_scheduler.m_threads[(std::size_t)coroutine->libcr_thread].local_cv.wait().libcr_wait(coroutine);
		}
	}

	template<class MtCV, class SyncCV>
	constexpr typename HybridScheduler<MtCV, SyncCV>::EnqueueCall HybridScheduler<MtCV, SyncCV>::enqueue()
	{
		return this;
	}

	template<class MtCV, class SyncCV>
	HybridScheduler<MtCV, SyncCV> &HybridScheduler<MtCV, SyncCV>::instance()
	{
		return s_instance;
	}
}