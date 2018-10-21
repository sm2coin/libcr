#include "Event.hpp"
#include <cassert>

namespace cr::sync
{
	template<class ConditionVariable>
	void PODEventBase<ConditionVariable>::initialise()
	{
		m_cv.initialise();
		m_happened = false;
	}

	template<class ConditionVariable>
	void PODEventBase<ConditionVariable>::fire()
	{
		assert(!m_happened);
		m_happened = true;
		m_cv.notify_all();
	}

	template<class ConditionVariable>
	void PODEventBase<ConditionVariable>::clear()
	{
		assert(m_happened);
		m_happened = false;
	}

	template<class ConditionVariable>
	mayblock PODEventBase<ConditionVariable>::WaitCall::libcr_wait(
		Coroutine * coroutine)
	{
		if(m_event.m_happened)
			return nonblock();
		else
		{
			return m_event.m_cv.wait().libcr_wait(coroutine);
		}
	}

	template<class ConditionVariable>
	bool PODEventBase<ConditionVariable>::happened() const
	{
		return m_happened;
	}

	template<class ConditionVariable>
	EventBase<ConditionVariable>::EventBase()
	{
		PODEventBase<ConditionVariable>::initialise();
	}

	template<class ConditionVariable>
	void PODConsumableEventBase<ConditionVariable>::fire()
	{
		assert(!m_happened);
		if(!m_cv.notify_one())
			m_happened = true;
	}

	template<class ConditionVariable>
	ConsumableEventBase<ConditionVariable>::ConsumableEventBase()
	{
		PODConsumableEventBase<ConditionVariable>::initialise();
	}

	template<class ConditionVariable>
	mayblock PODConsumableEventBase<ConditionVariable>::ConsumeCall::libcr_wait(
		Coroutine * coroutine)
	{
		if(m_event.m_happened)
		{
			m_event.clear();
			return nonblock();
		} else
		{
			return m_event.m_cv.wait().libcr_wait(coroutine);
		}
	}

	template class PODEventBase<PODConditionVariable>;
	template class PODEventBase<PODFIFOConditionVariable>;
	template class EventBase<PODConditionVariable>;
	template class EventBase<PODFIFOConditionVariable>;
	template class PODConsumableEventBase<PODConditionVariable>;
	template class PODConsumableEventBase<PODFIFOConditionVariable>;
	template class ConsumableEventBase<PODConditionVariable>;
	template class ConsumableEventBase<PODFIFOConditionVariable>;
}