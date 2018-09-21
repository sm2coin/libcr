namespace cr::sync
{
	bool PODMutex::is_owner(
		Coroutine * self)
	{
		return self == m_owner;
	}

	constexpr PODMutex::LockCall::LockCall(
		PODMutex &mutex):
		m_mutex(mutex)
	{
	}

	bool PODMutex::LockCall::wait(
		Coroutine * self)
	{
		return m_mutex.lock(self);
	}

	constexpr PODMutex::LockCall PODMutex::lock()
	{
		return LockCall(*this);
	}
}