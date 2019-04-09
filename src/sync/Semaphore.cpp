#include "Semaphore.hpp"

namespace cr::sync
{
	template<class ConditionVariable>
	void PODSemaphoreBase<ConditionVariable>::initialise(
		std::size_t counter)
	{
		m_cv.initialise();
		m_counter = counter;
	}

	template<class ConditionVariable>
	mayblock PODSemaphoreBase<ConditionVariable>::WaitCall::libcr_wait(
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
	void PODSemaphoreBase<ConditionVariable>::notify()
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
	SemaphoreBase<ConditionVariable>::SemaphoreBase(
		std::size_t counter)
	{
		PODSemaphoreBase<ConditionVariable>::initialise(counter);
	}

	template class PODSemaphoreBase<PODConditionVariable>;
	template class PODSemaphoreBase<PODFIFOConditionVariable>;
	template class SemaphoreBase<PODConditionVariable>;
	template class SemaphoreBase<PODFIFOConditionVariable>;
}