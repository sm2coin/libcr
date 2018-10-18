#include "Semaphore.hpp"

namespace cr::sync
{
	template<class ConditionVariable>
	void PODSemaphore<ConditionVariable>::initialise(
		std::size_t counter)
	{
		m_cv.initialise();
		m_counter = counter;
	}

	template<class ConditionVariable>
	mayblock PODSemaphore<ConditionVariable>::WaitCall::libcr_wait(
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

	template<class ConditionVariable>
	void PODSemaphore<ConditionVariable>::notify()
	{
		if(m_cv.empty())
		{
			++m_counter;
		} else
		{
			m_cv.notify_one();
		}
	}

	template<class ConditionVariable>
	Semaphore<ConditionVariable>::Semaphore(
		std::size_t counter)
	{
		PODSemaphore<ConditionVariable>::initialise(counter);
	}

	template class PODSemaphore<PODConditionVariable>;
	template class PODSemaphore<PODFIFOConditionVariable>;
	template class Semaphore<PODConditionVariable>;
	template class Semaphore<PODFIFOConditionVariable>;
}