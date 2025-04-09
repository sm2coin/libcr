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

	constexpr PODMutex::LockCall PODMutex::lock()
	{
		return LockCall(*this);
	}
}

#ifdef LIBCR_INLINE
#define LIBCR_SYNC_MUTEX_INLINE
#include "Mutex.cpp"
#endif