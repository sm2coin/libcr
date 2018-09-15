#include "Mutex.hpp"
#include "../Coroutine.hpp"

namespace cr::sync
{
	bool PODMutex::lock(
		Coroutine * self)
	{
		if(!m_owner)
		{
			m_owner = self;
			return true;
		} else
		{
			m_cv.wait(self);
			return false;
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