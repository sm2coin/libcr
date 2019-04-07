namespace cr::mt
{
	constexpr PODFIFOConditionVariable::WaitCall::WaitCall(
		PODFIFOConditionVariable &cv,
		bool invalidate_thread):
		m_cv(cv),
		m_invalidate_thread(invalidate_thread)
	{
	}

	constexpr PODFIFOConditionVariable::WaitCall PODFIFOConditionVariable::wait(
		bool invalidate_thread)
	{
		return WaitCall(*this, invalidate_thread);
	}

	constexpr PODConditionVariable::WaitCall::WaitCall(
		PODConditionVariable &cv,
		bool invalidate_thread):
		m_cv(cv),
		m_invalidate_thread(invalidate_thread)
	{
	}

	constexpr PODConditionVariable::WaitCall PODConditionVariable::wait(
		bool invalidate_thread)
	{
		return WaitCall(*this, invalidate_thread);
	}
}