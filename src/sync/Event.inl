namespace cr::sync
{
	template<class ConditionVariable>
	constexpr PODEventPattern<ConditionVariable>::WaitCall::WaitCall(
		PODEventPattern<ConditionVariable> &event):
		m_event(event)
	{
	}

	template<class ConditionVariable>
	constexpr
		typename PODEventPattern<ConditionVariable>::WaitCall
		PODEventPattern<ConditionVariable>::wait()
	{
		return WaitCall(*this);
	}

	template<class ConditionVariable>
	constexpr PODConsumableEventPattern<ConditionVariable>::ConsumeCall::ConsumeCall(
		PODConsumableEventPattern<ConditionVariable> &event):
		m_event(event)
	{
	}

	template<class ConditionVariable>
	constexpr
		typename PODConsumableEventPattern<ConditionVariable>::ConsumeCall
		PODConsumableEventPattern<ConditionVariable>::consume()
	{
		return ConsumeCall(*this);
	}
}