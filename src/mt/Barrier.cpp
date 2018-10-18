#include "Barrier.hpp"

namespace cr::mt
{
	template<class ConditionVariable>
	void PODBarrierBase<ConditionVariable>::initialise()
	{
		m_cv.initialise();
		m_count.store(0, std::memory_order_relaxed);
	}

	template<class ConditionVariable>
	void PODBarrierBase<ConditionVariable>::initialise(
		std::size_t count)
	{
		m_cv.initialise();
		m_count.store(count, std::memory_order_relaxed);
	}

	template<class ConditionVariable>
	void PODBarrierBase<ConditionVariable>::set(
		std::size_t count)
	{
		m_count.store(count, std::memory_order_relaxed);
	}

	template<class ConditionVariable>
	sync::mayblock PODBarrierBase<ConditionVariable>::WaitCall::libcr_wait(
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