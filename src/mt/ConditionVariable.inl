namespace cr::mt
{
	bool PODFIFOConditionVariable::empty() const
	{
		return !m_first_waiting.load(std::memory_order_relaxed);
	}

	constexpr PODFIFOConditionVariable::WaitCall::WaitCall(
		PODFIFOConditionVariable &cv):
		m_cv(cv)
	{
	}

	constexpr PODFIFOConditionVariable::WaitCall PODFIFOConditionVariable::wait()
	{
		return WaitCall(*this);
	}

	bool PODConditionVariable::empty() const
	{
		return !m_waiting.load(std::memory_order_relaxed);
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