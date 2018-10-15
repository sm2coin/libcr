namespace cr::mt
{
	constexpr Barrier::WaitCall::WaitCall(
		Barrier &barrier):
		m_barrier(barrier)
	{
	}

	constexpr Barrier::WaitCall Barrier::wait()
	{
		return WaitCall(*this);
	}
}