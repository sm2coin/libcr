namespace cr::mt
{
	template<class ConditionVariable>
	constexpr PODBarrierPattern<ConditionVariable>::WaitCall::WaitCall(
		PODBarrierPattern<ConditionVariable> &barrier):
		m_barrier(barrier)
	{
	}

	template<class ConditionVariable>
	constexpr typename PODBarrierPattern<ConditionVariable>::WaitCall PODBarrierPattern<ConditionVariable>::wait()
	{
		return WaitCall(*this);
	}
}