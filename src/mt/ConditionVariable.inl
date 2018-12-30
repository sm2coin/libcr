namespace cr::mt
{
	constexpr PODFIFOConditionVariable::WaitCall::WaitCall(
		PODFIFOConditionVariable &cv):
		m_cv(cv)
	{
	}

	constexpr PODFIFOConditionVariable::WaitCall PODFIFOConditionVariable::wait()
	{
		return WaitCall(*this);
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