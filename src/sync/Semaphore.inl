namespace cr::sync
{
	template<class ConditionVariable>
	constexpr PODSemaphoreBase<ConditionVariable>::WaitCall::WaitCall(
		PODSemaphoreBase<ConditionVariable> &semaphore):
		m_semaphore(semaphore)
	{
	}

	template<class ConditionVariable>
	constexpr typename PODSemaphoreBase<ConditionVariable>::WaitCall PODSemaphoreBase<ConditionVariable>::wait()
	{
		return WaitCall(*this);
	}
}