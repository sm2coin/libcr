namespace cr::sync
{
	constexpr PODBarrier::WaitCall::WaitCall(
		PODBarrier &barrier):
		m_barrier(barrier)
	{
	}

	constexpr PODBarrier::WaitCall PODBarrier::wait()
	{
		return WaitCall(*this);
	}
}