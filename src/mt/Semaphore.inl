namespace cr::mt
{
	template<class ConditionVariable>
	constexpr PODSemaphorePattern<ConditionVariable>::WaitCall::WaitCall(
		PODSemaphorePattern<ConditionVariable> * semaphore):
		m_semaphore(*semaphore)
	{
	}

	template<class ConditionVariable>
	constexpr typename PODSemaphorePattern<ConditionVariable>::WaitCall PODSemaphorePattern<ConditionVariable>::wait()
	{
		return this;
	}

	template<class ConditionVariable>
	SemaphorePattern<ConditionVariable>::SemaphorePattern()
	{
		initialise(0);
	}

	template<class ConditionVariable>
	SemaphorePattern<ConditionVariable>::SemaphorePattern(
		std::size_t count)
	{
		initialise(count);
	}
}