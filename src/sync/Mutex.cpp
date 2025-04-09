#ifndef __libcr_sync_mutex_cpp_defined
#define __libcr_sync_mutex_cpp_defined

#ifndef LIBCR_SYNC_MUTEX_INLINE
#include "Mutex.hpp"
#else
#undef LIBCR_SYNC_MUTEX_INLINE
#endif
#include "../Coroutine.hpp"
#include "Promise.hpp"

namespace cr::sync
{
	void PODMutex::initialise()
	{
		m_cv.initialise();
		m_owner = nullptr;
	}

	mayblock PODMutex::LockCall::libcr_wait(
		Coroutine * self)
	{
		if(!m_mutex.m_owner)
		{
			m_mutex.m_owner = self;
			return nonblock();
		} else
		{
			return m_mutex.m_cv.wait().libcr_wait(self);
		}
	}

	bool PODMutex::try_lock(
		Coroutine * self)
	{
		if(!m_owner)
		{
			m_owner = self;
			return true;
		} else
		{
			return false;
		}
	}

	void PODMutex::unlock()
	{
		assert(m_owner != nullptr);

		m_owner = m_cv.front();
		m_cv.notify_one();
	}

	Mutex::Mutex()
	{
		initialise();
	}
}

#endif