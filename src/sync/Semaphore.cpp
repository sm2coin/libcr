#include "Semaphore.hpp"

namespace cr::sync
{
	void PODSemaphore::initialise(
		std::size_t counter)
	{
		m_cv.initialise();
		m_counter = counter;
	}

	mayblock PODSemaphore::WaitCall::libcr_wait(
		Coroutine * coroutine)
	{
		if(m_semaphore.m_counter == 0)
			return m_semaphore.m_cv.wait().libcr_wait(coroutine);
		else
		{
			--m_semaphore.m_counter;
			return nonblock();
		}
	}

	void PODSemaphore::notify()
	{
		if(m_cv.empty())
		{
			++m_counter;
		} else
		{
			m_cv.notify_one();
		}
	}

	Semaphore::Semaphore(
		std::size_t counter)
	{
		PODSemaphore::initialise(counter);
	}
}