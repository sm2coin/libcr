#include "SoftMutex.hpp"

namespace cr::mt::detail
{
	void PODSoftMutex::lock()
	{
		bool value;
		do {
			value = false;
		} while(!m_locked.compare_exchange_weak(
			value,
			true,
			std::memory_order_acquire,
			std::memory_order_relaxed));
	}

	bool PODSoftMutex::try_lock()
	{
		bool value = false;
		return m_locked.compare_exchange_strong(
			value,
			true,
			std::memory_order_acquire,
			std::memory_order_relaxed);
	}

	void PODSoftMutex::unlock()
	{
		assert(m_locked.load(std::memory_order_relaxed));
		m_locked.store(false, std::memory_order_release);
	}

	LockGuard::~LockGuard()
	{
		if(m_mutex)
		{
			m_mutex->unlock();
			m_mutex = nullptr;
		}
	}
}