namespace cr::sync
{
	bool PODFIFOConditionVariable::empty() const
	{
		return m_first_waiting == nullptr;
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

	Coroutine * PODFIFOConditionVariable::front()
	{
		return m_first_waiting;
	}

	bool PODConditionVariable::empty() const
	{
		return m_waiting == nullptr;
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
		return m_waiting;
	}
}

#ifdef LIBCR_INLINE
#define LIBCR_SYNC_CONDITIONVARIABLE_INLINE
#include "ConditionVariable.cpp"
#endif