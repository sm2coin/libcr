#include "Barrier.hpp"
#include "../Coroutine.hpp"

namespace cr::mt
{
	template<class ConditionVariable>
	void PODBarrierBase<ConditionVariable>::initialise()
	{
		m_cv.initialise();
		std::atomic_init(&m_count, (std::size_t) 0);
	}

	template<class ConditionVariable>
	void PODBarrierBase<ConditionVariable>::initialise(
		std::size_t count)
	{
		m_cv.initialise();
		std::atomic_init(&m_count, count);
	}

	template<class ConditionVariable>
	void PODBarrierBase<ConditionVariable>::set(
		std::size_t count)
	{
		m_count.store(count, std::memory_order_relaxed);
		if(!count)
			m_cv.notify_all();
	}

	template<class ConditionVariable>
	sync::mayblock PODBarrierBase<ConditionVariable>::WaitCall::libcr_wait(
		Coroutine * coroutine)
	{
		std::size_t count = 1;

		// Decrement the barrier's counter.
		while(!m_barrier.m_count.compare_exchange_weak(
			count,
			count - 1,
			std::memory_order_acq_rel,
			std::memory_order_relaxed))
		{
			// If the barrier was unblocked in the meantime, return.
			if(!count)
			{
				return sync::nonblock();
			}
		}

		// Did this coroutine unblock the barrier?
		if(count == 1)
		{
			m_barrier.m_cv.notify_all();
			return sync::nonblock();
		} else
		{
			return coroutine->libcr_unpack_wait(m_barrier.m_cv.wait());
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