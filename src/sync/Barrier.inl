namespace cr::sync
{
	template<class ConditionVariable>
	constexpr PODBarrierBase<ConditionVariable>::WaitCall::WaitCall(
		PODBarrierBase<ConditionVariable> &barrier):
		m_barrier(barrier)
	{
	}

	template<class ConditionVariable>
	constexpr typename PODBarrierBase<ConditionVariable>::WaitCall PODBarrierBase<ConditionVariable>::wait()
	{
		return WaitCall(*this);
	}
}