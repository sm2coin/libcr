#include "Barrier.hpp"

namespace cr::mt
{
	void PODBarrier::initialise()
	{
		m_cv.initialise();
		m_count.store(0, std::memory_order_relaxed);
	}

	void PODBarrier::initialise(
		std::size_t count)
	{
		m_cv.initialise();
		m_count.store(count, std::memory_order_relaxed);
	}

	void PODBarrier::set(
		std::size_t count)
	{
		m_count.store(count, std::memory_order_relaxed);
	}

	sync::mayblock PODBarrier::WaitCall::libcr_wait(
		Coroutine * coroutine)
	{
		std::size_t count = m_barrier.m_count.load(std::memory_order_relaxed);

		// Decrement the barrier's counter.
		do {
			// If the barrier was unblocked in the meantime, return.
			if(!count)
				return sync::nonblock();
		} while(!m_barrier.m_count.compare_exchange_weak(
			count,
			count - 1,
			std::memory_order_relaxed));

		// Did this coroutine unblock the barrier?
		if(count == 1)
		{
			m_barrier.m_cv.notify_all();
			return sync::nonblock();
		} else
		{
			return m_barrier.m_cv.wait().libcr_wait(coroutine);
		}
	}

	Barrier::Barrier()
	{
		PODBarrier::initialise();
	}

	Barrier::Barrier(
		std::size_t count)
	{
		PODBarrier::initialise(count);
	}
}