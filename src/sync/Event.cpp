#ifndef __libcr_sync_event_cpp_defined
#define __libcr_sync_event_cpp_defined

#ifndef LIBCR_SYNC_EVENT_INLINE
#include "Event.hpp"
#else
#undef LIBCR_SYNC_EVENT_INLINE
#endif

#include <cassert>

namespace cr::sync
{
	template<class ConditionVariable>
	void PODEventPattern<ConditionVariable>::initialise()
	{
		m_cv.initialise();
		m_happened = false;
	}

	template<class ConditionVariable>
	void PODEventPattern<ConditionVariable>::fire()
	{
		assert(!m_happened);
		m_happened = true;
		m_cv.notify_all();
	}

	template<class ConditionVariable>
	void PODEventPattern<ConditionVariable>::clear()
	{
		assert(m_happened);
		m_happened = false;
	}

	template<class ConditionVariable>
	mayblock PODEventPattern<ConditionVariable>::WaitCall::libcr_wait(
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
	bool PODEventPattern<ConditionVariable>::happened() const
	{
		return m_happened;
	}

	template<class ConditionVariable>
	EventPattern<ConditionVariable>::EventPattern()
	{
		PODEventPattern<ConditionVariable>::initialise();
	}

	template<class ConditionVariable>
	void PODConsumableEventPattern<ConditionVariable>::fire()
	{
		assert(!m_happened);
		if(!m_cv.notify_one())
			m_happened = true;
	}

	template<class ConditionVariable>
	ConsumableEventPattern<ConditionVariable>::ConsumableEventPattern()
	{
		PODConsumableEventPattern<ConditionVariable>::initialise();
	}

	template<class ConditionVariable>
	mayblock PODConsumableEventPattern<ConditionVariable>::ConsumeCall::libcr_wait(
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

	template class PODEventPattern<PODConditionVariable>;
	template class PODEventPattern<PODFIFOConditionVariable>;
	template class EventPattern<PODConditionVariable>;
	template class EventPattern<PODFIFOConditionVariable>;
	template class PODConsumableEventPattern<PODConditionVariable>;
	template class PODConsumableEventPattern<PODFIFOConditionVariable>;
	template class ConsumableEventPattern<PODConditionVariable>;
	template class ConsumableEventPattern<PODFIFOConditionVariable>;
}

#endif