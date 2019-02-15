#include <cassert>

namespace cr::mt::detail
{
	void PODSoftMutex::initialise()
	{
		std::atomic_init(&m_locked, false);
	}

	SoftMutex::SoftMutex()
	{
		initialise();
	}

	LockGuard::LockGuard():
		m_mutex(nullptr)
	{
	}

	LockGuard::LockGuard(
		PODSoftMutex &mutex)
	{
		mutex.lock();
		m_mutex = &mutex;
	}

	void LockGuard::unlock()
	{
		assert(m_mutex != nullptr);
		m_mutex->unlock();
	}

	void LockGuard::lock(
		PODSoftMutex &mutex)
	{
		assert(!locked());
		m_mutex = &mutex;
		mutex.lock();
	}

	bool LockGuard::try_lock(
		PODSoftMutex &mutex)
	{
		assert(!locked());
		if(mutex.try_lock())
			m_mutex = &mutex;

		return locked();
	}

	bool LockGuard::locked() const
	{
		return m_mutex != nullptr;
	}
}