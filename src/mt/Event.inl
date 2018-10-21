namespace cr::mt
{
	template<class ConditionVariable>
	constexpr PODEventBase<ConditionVariable>::WaitCall::WaitCall(
		PODEventBase<ConditionVariable> &event):
		m_event(event)
	{
	}

	template<class ConditionVariable>
	constexpr typename PODEventBase<ConditionVariable>::WaitCall PODEventBase<ConditionVariable>::wait()
	{
		return WaitCall(*this);
	}

	template<class ConditionVariable>
	constexpr PODConsumableEventBase<ConditionVariable>::ConsumeCall::ConsumeCall(
		PODConsumableEventBase<ConditionVariable> &event):
		m_event(event)
	{
	}

	template<class ConditionVariable>
	constexpr typename PODConsumableEventBase<ConditionVariable>::ConsumeCall PODConsumableEventBase<ConditionVariable>::consume()
	{
		return WaitCall(*this);
	}
}