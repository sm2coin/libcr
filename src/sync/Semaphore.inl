namespace cr::sync
{
	template<class ConditionVariable>
	constexpr PODSemaphorePattern<ConditionVariable>::WaitCall::WaitCall(
		PODSemaphorePattern<ConditionVariable> &semaphore):
		m_semaphore(semaphore)
	{
	}

	template<class ConditionVariable>
	constexpr typename PODSemaphorePattern<ConditionVariable>::WaitCall PODSemaphorePattern<ConditionVariable>::wait()
	{
		return WaitCall(*this);
	}
}