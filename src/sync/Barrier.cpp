#include "Barrier.hpp"

namespace cr::sync
{
	void PODBarrier::initialise()
	{
		m_cv.initialise();
		m_count = 0;
	}

	void PODBarrier::initialise(
		std::size_t count)
	{
		m_cv.initialise();
		m_count = count;
	}

	void PODBarrier::set(
		std::size_t count)
	{
		m_count = count;
	}

	mayblock PODBarrier::WaitCall::libcr_wait(
		Coroutine * coroutine)
	{
		if(m_barrier.m_count)
		{
			if(!--m_barrier.m_count)
			{
				m_barrier.m_cv.notify_all();
				return nonblock();
			} else
			{
				return m_barrier.m_cv.wait().libcr_wait(coroutine);
			}
		} else
			return nonblock();
	}

	Barrier::Barrier()
	{
		initialise();
	}

	Barrier::Barrier(
		std::size_t count)
	{
		initialise(count);
	}
}