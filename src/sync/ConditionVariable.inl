namespace cr::sync
{
	bool PODConditionVariable::empty() const
	{
		return m_first_waiting == nullptr;
	}

	constexpr PODConditionVariable::WaitCall::WaitCall(
		PODConditionVariable &cv):
		m_cv(cv)
	{
	}

	constexpr PODConditionVariable::WaitCall PODConditionVariable::wait()
	{
		return WaitCall(*this);
	}

	Coroutine * PODConditionVariable::front()
	{
		return m_first_waiting;
	}

	bool PODSingleConditionVariable::empty() const
	{
		return m_waiting == nullptr;
	}

	constexpr PODSingleConditionVariable::WaitCall::WaitCall(
		PODSingleConditionVariable &cv):
		m_cv(cv)
	{
	}

	constexpr PODSingleConditionVariable::WaitCall PODSingleConditionVariable::wait()
	{
		return WaitCall(*this);
	}

	Coroutine * PODSingleConditionVariable::front()
	{
		return m_waiting;
	}
}