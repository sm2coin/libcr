namespace cr::sync
{
	template<class ConditionVariable>
	constexpr PODSemaphore<ConditionVariable>::WaitCall::WaitCall(
		PODSemaphore &semaphore):
		m_semaphore(semaphore)
	{
	}

	template<class ConditionVariable>
	constexpr typename PODSemaphore<ConditionVariable>::WaitCall PODSemaphore<ConditionVariable>::wait()
	{
		return WaitCall(*this);
	}
}