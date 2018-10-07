namespace cr::sync
{
	constexpr PODSemaphore::WaitCall::WaitCall(
		PODSemaphore &semaphore):
		m_semaphore(semaphore)
	{
	}

	constexpr PODSemaphore::WaitCall PODSemaphore::wait()
	{
		return WaitCall(*this);
	}
}