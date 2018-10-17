namespace cr::mt
{
	bool PODConditionVariable::empty() const
	{
		return m_first_waiting.load(std::memory_order_relaxed);
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
}