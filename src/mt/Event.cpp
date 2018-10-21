#include "Event.hpp"

namespace cr::mt
{
	template<class ConditionVariable>
	void PODEventBase<ConditionVariable>::initialise()
	{
		m_cv.initialise();
		std::atomic_init(&m_active, false);
	}

	template<class ConditionVariable>
	void PODEventBase<ConditionVariable>::fire()
	{
		m_active.store(true, std::memory_order_relaxed);
		m_cv.notify_all();
	}

	template<class ConditionVariable>
	void PODEventBase<ConditionVariable>::clear()
	{
		m_active.store(false, std::memory_order_relaxed);
	}

	template<class ConditionVariable>
	sync::mayblock PODEventBase<ConditionVariable>::WaitCall::libcr_wait(
		Coroutine * coroutine)
	{
		if(!m_event.m_active.load(std::memory_order_relaxed))
			return m_cv.wait().libcr_wait(coroutine);
		else
			return sync::nonblock();
	}

	template<class ConditionVariable>
	EventBase<ConditionVariable>::EventBase()
	{
		PODEventBase<ConditionVariable>::initialise();
	}

	template<class ConditionVariable>
	void PODConsumableEventBase<ConditionVariable>::fire()
	{
		if(!m_cv.notify_one())
			m_active.store(true, std::memory_order_relaxed);
	}

	template<class ConditionVariable>
	sync::mayblock PODConsumableEventBase<ConditionVariable>::ConsumeCall::libcr_wait(
		Coroutine * coroutine)
	{
		bool active = true;
		if(m_active.compare_exchange_strong(
			active,
			false,
			std::memory_order_relaxed))
		{
			return sync::nonblock();
		} else
		{
			return m_cv.wait().libcr_wait(coroutine);
		}
	}

	template<class ConditionVariable>
	ConsumableEventBase<ConditionVariable>::ConsumableEventBase()
	{
		PODConsumableEventBase<ConditionVariable>::initialise();
	}
}