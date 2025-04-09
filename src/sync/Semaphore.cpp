#ifndef __libcr_sync_semaphore_cpp_defined
#define __libcr_sync_semaphore_cpp_defined

#ifndef LIBCR_SYNC_SEMAPHORE_INLINE
#include "Semaphore.hpp"
#else
#undef LIBCR_SYNC_SEMAPHORE_INLINE
#endif

namespace cr::sync
{
	template<class ConditionVariable>
	void PODSemaphorePattern<ConditionVariable>::initialise(
		std::size_t counter)
	{
		m_cv.initialise();
		m_counter = counter;
	}

	template<class ConditionVariable>
	mayblock PODSemaphorePattern<ConditionVariable>::WaitCall::libcr_wait(
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
	bool PODSemaphorePattern<ConditionVariable>::notify()
	{
		bool notified = m_cv.notify_one();
		if(!notified)
			++m_counter;

		return notified;
	}

	template<class ConditionVariable>
	SemaphorePattern<ConditionVariable>::SemaphorePattern(
		std::size_t counter)
	{
		PODSemaphorePattern<ConditionVariable>::initialise(counter);
	}

	template class PODSemaphorePattern<PODConditionVariable>;
	template class PODSemaphorePattern<PODFIFOConditionVariable>;
	template class SemaphorePattern<PODConditionVariable>;
	template class SemaphorePattern<PODFIFOConditionVariable>;
}

#endif