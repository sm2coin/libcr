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
	bool PODEventBase<ConditionVariable>::wait(
		Coroutine * coroutine)
	{
		if(m_happened)
			return true;
		else
		{
			m_cv.wait(coroutine);
			return false;
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
}