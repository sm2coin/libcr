#include "Barrier.hpp"

namespace cr::sync
{
	template<class ConditionVariable>
	void PODBarrierPattern<ConditionVariable>::initialise()
	{
		m_cv.initialise();
		m_count = 0;
	}

	template<class ConditionVariable>
	void PODBarrierPattern<ConditionVariable>::initialise(
		std::size_t count)
	{
		m_cv.initialise();
		m_count = count;
	}

	template<class ConditionVariable>
	void PODBarrierPattern<ConditionVariable>::set(
		std::size_t count)
	{
		m_count = count;
	}

	template<class ConditionVariable>
	mayblock PODBarrierPattern<ConditionVariable>::WaitCall::libcr_wait(
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

	template<class ConditionVariable>
	BarrierPattern<ConditionVariable>::BarrierPattern()
	{
		PODBarrierPattern<ConditionVariable>::initialise();
	}

	template<class ConditionVariable>
	BarrierPattern<ConditionVariable>::BarrierPattern(
		std::size_t count)
	{
		PODBarrierPattern<ConditionVariable>::initialise(count);
	}

	template class PODBarrierPattern<PODConditionVariable>;
	template class PODBarrierPattern<PODFIFOConditionVariable>;
	template class BarrierPattern<PODConditionVariable>;
	template class BarrierPattern<PODFIFOConditionVariable>;
}