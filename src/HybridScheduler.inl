#include <timer/Timer.hpp>
#include <thread>
#include <cstdlib>

namespace cr
{
	template<class MtCV, class SyncCV>
	HybridScheduler<MtCV, SyncCV> HybridScheduler<MtCV, SyncCV>::s_instance;

	template<class MtCV, class SyncCV>
	HybridScheduler<MtCV, SyncCV>::ThreadContext::ThreadContext():
		global_cv(),
		local_cv(),
		load((~(time_t)0)>>11), // prevent overflow
		rng(rand())
	{
	}

	template<class MtCV, class SyncCV>
	void HybridScheduler<MtCV, SyncCV>::initialise(
		std::size_t threads)
	{
		if(threads < 2)
			threads = 1;
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
		m_busy_thread(1),
		m_idle_thread(0)
	{
	}

	template<class MtCV, class SyncCV>
	bool HybridScheduler<MtCV, SyncCV>::schedule(
		std::size_t thread)
	{
		std::size_t busy;
		bool balance = false;
		std::uint8_t balance_odds;
		std::size_t idle_thread;

		if(m_threads.size() != 1)
		{
			busy = m_busy_thread.load_weak(std::memory_order_relaxed);
			balance = busy == thread;
			if(balance)
			{
				idle_thread = m_idle_thread.load_weak(std::memory_order_relaxed);
				if((balance = idle_thread != busy))
				{
					auto idle_time = m_threads[idle_thread].load.load_weak(std::memory_order_relaxed);
					auto busy_time = m_threads[busy].load.load_weak(std::memory_order_relaxed);
					balance_odds = (time_t(256)*busy_time) / (busy_time + idle_time + time_t(1));
					if(balance_odds < 3)
						balance = false;
				}
			}
		}

		ThreadContext &ctx = m_threads[thread];
		Coroutine * gfirst, * glast;
		bool result = ctx.global_cv.remove_all(gfirst, glast);

		timer::Timer<std::chrono::microseconds> timer;
		Coroutine * lfirst;
		if((lfirst = ctx.local_cv.remove_all()))
			result = true;

		Coroutine * next;

		Coroutine * q_first = nullptr, * q_last = nullptr;

		timer.start();

		while(lfirst)
		{
			next = lfirst->libcr_next_waiting.plain;
			if(balance && ctx.rng.flip(balance_odds))
			{
				lfirst->libcr_thread = (detail::Thread) idle_thread;
				if(!q_first)
					q_first = lfirst;
				else
					q_last->libcr_next_waiting.plain = lfirst;
				q_last = lfirst;
			}
			else
				(*lfirst)();
			lfirst = next;
		}

		while(gfirst)
		{
			next = MtCV::acquire_and_complete(gfirst, glast);
			if(balance && ctx.rng.flip(balance_odds))
			{
				gfirst->libcr_thread = (detail::Thread) idle_thread;
				if(!q_first)
					q_first = gfirst;
				else
					q_last->libcr_next_waiting.plain = gfirst;
				q_last = gfirst;
			}
			else
				(*gfirst)();
			gfirst = next;
		}

		if(q_first)
			(void)m_threads[idle_thread].global_cv.wait(false).libcr_wait(q_first, q_last);

		// The first thread updates the statistics.
		if(m_threads.size() != 1)
		{
			if(thread == 0)
				detect_load();
			ctx.load.store(timer.stop(), std::memory_order_relaxed);
		}

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