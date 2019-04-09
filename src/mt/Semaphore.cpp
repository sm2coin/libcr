#include "Semaphore.hpp"

#include "ConditionVariable.hpp"

#include "../Coroutine.hpp"

namespace cr::mt
{
	template<class ConditionVariable>
	void PODSemaphoreBase<ConditionVariable>::initialise(
		std::size_t count)
	{
		m_cv.initialise();
		std::atomic_init(&m_count, count);
		std::atomic_init(&m_registering, (std::size_t) 0);
		m_mutex.initialise();
	}

	template<class ConditionVariable>
	void PODSemaphoreBase<ConditionVariable>::notify()
	{
		if(m_cv.notify_one())
			return;

		// Try to increment the semaphore (safe if it is > 0).
		std::size_t count = 1;
		while(count)
			if(m_count.compare_exchange_weak(
				count,
				count + 1,
				std::memory_order_acq_rel,
				std::memory_order_relaxed))
				return;

		for(;;)
		{
			// If there are registering coroutines, try again.
			if(m_registering.load_strong(std::memory_order_relaxed))
				continue;

			// Otherwise, lock the semaphore.
			detail::LockGuard lock { m_mutex };

			// If there are registering coroutines, try again.
			if(m_registering.load_strong(std::memory_order_relaxed))
				continue;

			// Once more, try to remove a coroutine.
			Coroutine * removed = m_cv.remove_one();
			if(removed)
			{
				lock.unlock();
				m_cv.acquire_and_complete(removed, removed);
				(*removed)();
				return;
			} else
			{
				// Otherwise, notify the semaphore.
				m_count.fetch_add(1, std::memory_order_acq_rel);
				return;
			}
		}
	}

	template<class ConditionVariable>
	sync::mayblock PODSemaphoreBase<ConditionVariable>::WaitCall::libcr_wait(
		Coroutine * coroutine)
	{
		if(m_semaphore.try_wait())
			return sync::nonblock();

		detail::LockGuard lock { m_semaphore.m_mutex };
		// Was the semaphore increased in the meantime?
		if(m_semaphore.try_wait())
			return sync::nonblock();

		m_semaphore.m_registering.fetch_add(1, std::memory_order_relaxed);
		lock.unlock();

		(void) m_semaphore.m_cv.wait().libcr_wait(coroutine);

		// Release, because the operation must not be reordered.
		m_semaphore.m_registering.fetch_sub(1, std::memory_order_release);
		return sync::block();
	}

	template<class ConditionVariable>
	bool PODSemaphoreBase<ConditionVariable>::try_wait()
	{
		std::size_t count = 1;
		while(count)
			if(m_count.compare_exchange_weak(
				count,
				count-1,
				std::memory_order_acq_rel,
				std::memory_order_relaxed))
			{
				return true;
			}

		return false;
	}

	template class PODSemaphoreBase<PODConditionVariable>;
	template class PODSemaphoreBase<PODFIFOConditionVariable>;
	template class SemaphoreBase<PODConditionVariable>;
	template class SemaphoreBase<PODFIFOConditionVariable>;
}