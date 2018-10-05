namespace cr::sync
{
	template<class ConditionVariable>
	void PODEventBase<ConditionVariable>::initialise()
	{
		m_cv.initialise();
		m_happened = false;
	}

	template<class ConditionVariable>
	bool PODEventBase<ConditionVariable>::listeners() const
	{
		return !m_cv.empty();
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
	constexpr PODEventBase<ConditionVariable>::WaitCall::WaitCall(
		PODEventBase<ConditionVariable> &event):
		m_event(event)
	{
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
	constexpr
		typename PODEventBase<ConditionVariable>::WaitCall
		PODEventBase<ConditionVariable>::wait()
	{
		return WaitCall(*this);
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
}