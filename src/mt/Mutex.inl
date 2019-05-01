namespace cr::mt
{
	template<class ConditionVariable>
	bool PODMutexPattern<ConditionVariable>::try_lock()
	{
		return PODConsumableEventPattern<ConditionVariable>::try_consume();
	}

	template<class ConditionVariable>
	typename PODConsumableEventPattern<ConditionVariable>::ConsumeCall PODMutexPattern<ConditionVariable>::lock()
	{
		return PODConsumableEventPattern<ConditionVariable>::consume();
	}

	template<class ConditionVariable>
	void PODMutexPattern<ConditionVariable>::unlock()
	{
		PODConsumableEventPattern<ConditionVariable>::clear();
	}

	template<class ConditionVariable>
	MutexPattern<ConditionVariable>::MutexPattern()
	{
		initialise();
	}
}