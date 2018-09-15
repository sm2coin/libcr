namespace cr::sync
{
	bool PODConditionVariable::empty()
	{
		return m_first_waiting == nullptr;
	}

	Coroutine * PODConditionVariable::front()
	{
		return m_first_waiting;
	}

	bool PODSingleConditionVariable::empty()
	{
		return m_waiting == nullptr;
	}

	Coroutine * PODSingleConditionVariable::front()
	{
		return m_waiting;
	}
}