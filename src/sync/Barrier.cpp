#include "Barrier.hpp"

namespace cr::sync
{
	template<class ConditionVariable>
	void PODBarrierBase<ConditionVariable>::initialise()
	{
		m_cv.initialise();
		m_count = 0;
	}

	template<class ConditionVariable>
	void PODBarrierBase<ConditionVariable>::initialise(
		std::size_t count)
	{
		m_cv.initialise();
		m_count = count;
	}

	template<class ConditionVariable>
	void PODBarrierBase<ConditionVariable>::set(
		std::size_t count)
	{
		m_count = count;
	}

	template<class ConditionVariable>
	mayblock PODBarrierBase<ConditionVariable>::WaitCall::libcr_wait(
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
	BarrierBase<ConditionVariable>::BarrierBase()
	{
		PODBarrierBase<ConditionVariable>::initialise();
	}

	template<class ConditionVariable>
	BarrierBase<ConditionVariable>::BarrierBase(
		std::size_t count)
	{
		PODBarrierBase<ConditionVariable>::initialise(count);
	}

	template class PODBarrierBase<PODConditionVariable>;
	template class PODBarrierBase<PODFIFOConditionVariable>;
	template class BarrierBase<PODConditionVariable>;
	template class BarrierBase<PODFIFOConditionVariable>;
}