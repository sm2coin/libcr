namespace cr::mt
{
	template<class ConditionVariable>
	bool PODMutexBase<ConditionVariable>::try_lock()
	{
		return PODConsumableEventBase<ConditionVariable>::try_consume();
	}

	template<class ConditionVariable>
	typename PODConsumableEventBase<ConditionVariable>::ConsumeCall PODMutexBase<ConditionVariable>::lock()
	{
		return PODConsumableEventBase<ConditionVariable>::consume();
	}

	template<class ConditionVariable>
	void PODMutexBase<ConditionVariable>::unlock()
	{
		PODConsumableEventBase<ConditionVariable>::clear();
	}

	template<class ConditionVariable>
	MutexBase<ConditionVariable>::MutexBase()
	{
		initialise();
	}
}