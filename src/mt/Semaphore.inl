namespace cr::mt
{
	template<class ConditionVariable>
	constexpr PODSemaphoreBase<ConditionVariable>::WaitCall::WaitCall(
		PODSemaphoreBase<ConditionVariable> * semaphore):
		m_semaphore(*semaphore)
	{
	}

	template<class ConditionVariable>
	constexpr typename PODSemaphoreBase<ConditionVariable>::WaitCall PODSemaphoreBase<ConditionVariable>::wait()
	{
		return this;
	}

	template<class ConditionVariable>
	SemaphoreBase<ConditionVariable>::SemaphoreBase()
	{
		initialise(0);
	}

	template<class ConditionVariable>
	SemaphoreBase<ConditionVariable>::SemaphoreBase(
		std::size_t count)
	{
		initialise(count);
	}
}