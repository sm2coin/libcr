#include "SoftMutex.hpp"

namespace cr::mt::detail
{
	void PODSoftMutex::lock()
	{
		bool value = false;
		while(!m_flag.compare_exchange_weak(
			value,
			true,
			std::memory_order_acquire,
			std::memory_order_relaxed))
		{
			value = false;
		}
	}

	bool PODSoftMutex::try_lock()
	{
		bool value = false;
		return m_flag.compare_exchange_strong(
			value,
			true,
			std::memory_order_acquire,
			std::memory_order_relaxed);
	}

	void PODSoftMutex::unlock()
	{
		bool was_locked = m_flag.exchange(false, std::memory_order_release);
		(void)was_locked;
		assert(was_locked);
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