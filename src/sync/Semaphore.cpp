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
	bool PODSemaphoreBase<ConditionVariable>::notify()
	{
		bool notified = m_cv.notify_one();
		if(!notified)
			++m_counter;

		return notified;
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