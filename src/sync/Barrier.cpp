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

	bool PODBarrier::wait(
		Coroutine * coroutine)
	{
		if(m_count)
		{
			if(!--m_count)
			{
				m_cv.notify_all();
				return true;
			} else
			{
				m_cv.wait(coroutine);
				return false;
			}
		} else
			return true;
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