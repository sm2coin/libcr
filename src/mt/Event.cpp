#include "Event.hpp"
#include "../Coroutine.hpp"

namespace cr::mt
{
	template<class ConditionVariable>
	void PODEventBase<ConditionVariable>::initialise(
		bool active)
	{
		m_cv.initialise();
		std::atomic_init(&m_active, active);
		std::atomic_init(&m_registering, (std::size_t) 0);
		m_mutex.initialise();
	}

	template<class ConditionVariable>
	void PODEventBase<ConditionVariable>::fire()
	{
		for(;;)
		{
			// If there are registering coroutines, try again.
			if(m_registering.load_strong(std::memory_order_relaxed))
				continue;

			// Otherwise, lock the event.
			detail::LockGuard lock { m_mutex };
			// If there are registering coroutines, try again.
			if(m_registering.load_strong(std::memory_order_relaxed))
				continue;

			// Otherwise, set the event to active.
			m_active.store(true, std::memory_order_release);
			break;
		}

		m_cv.notify_all();
	}

	template<class ConditionVariable>
	void PODEventBase<ConditionVariable>::clear()
	{
		m_active.store(false, std::memory_order_relaxed);
	}

	template<class ConditionVariable>
	sync::mayblock PODEventBase<ConditionVariable>::WaitCall::libcr_wait(
		Coroutine * coroutine)
	{
		// `compare_exchange_strong` is used to enforce stronger synchronisation than `load`.
		bool active = true;
		// Fetch whether the event is active now.
		m_event.m_active.compare_exchange_strong(
			active,
			active,
			std::memory_order_acquire, // Acquire if event is active.
			std::memory_order_relaxed);
		// Return if it is active.
		if(active)
			return sync::nonblock();

		// Lock the event.
		detail::LockGuard lock { m_event.m_mutex };
		// Fetch whether the event is active now.
		active = true;
		m_event.m_active.compare_exchange_strong(
			active,
			active,
			std::memory_order_acquire, // Acquire if event is active.
			std::memory_order_relaxed);
		// Return if it is active.
		if(active)
			return sync::nonblock();

		// Increment the registering count.
		m_event.m_registering.fetch_add(1, std::memory_order_relaxed);

		// Unlock the event.
		lock.unlock();

		// Register the coroutine.
		coroutine->libcr_unpack_wait(m_event.m_cv.wait());
		// Decrement the registering count.
		// Release because the operation must not be reordered.
		m_event.m_registering.fetch_sub(1, std::memory_order_release);
		return sync::block();
	}

	template<class ConditionVariable>
	EventBase<ConditionVariable>::EventBase()
	{
		PODEventBase<ConditionVariable>::initialise();
	}

	template<class ConditionVariable>
	void PODConsumableEventBase<ConditionVariable>::fire()
	{
		if(m_cv.notify_one())
			return;

		for(;;)
		{
			// If there are registering coroutines, try again.
			if(m_registering.load_strong(std::memory_order_relaxed))
				continue;

			// Otherwise, lock the event.
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
				// Otherwise, set the event to active.
				m_active.exchange(true, std::memory_order_release);
				return;
			}
		}
	}

	template<class ConditionVariable>
	sync::mayblock PODConsumableEventBase<ConditionVariable>::ConsumeCall::libcr_wait(
		Coroutine * coroutine)
	{
		// Set the event to inactive.
		// Return if the event was active.
		if(m_event.try_consume())
			return sync::nonblock();

		// Lock the event.
		detail::LockGuard lock { m_event.m_mutex };
		// Set the event to inactive.
		// Return if the event was active.
		if(m_event.try_consume())
			return sync::nonblock();

		// Increment the registering count.
		m_event.m_registering.fetch_add(1, std::memory_order_relaxed);
		// Unlock the event.
		lock.unlock();

		// Register the coroutine.
		(void) m_event.m_cv.wait().libcr_wait(coroutine);

		// Decrement the registering count.
		// Release, because the operation must not be reordered.
		m_event.m_registering.fetch_sub(1, std::memory_order_release);
		return sync::block();
	}

	template<class ConditionVariable>
	bool PODConsumableEventBase<ConditionVariable>::try_consume()
	{
		bool active = true;
		return m_active.compare_exchange_strong(
			active,
			false,
			std::memory_order_acquire,
			std::memory_order_relaxed);
	}

	template<class ConditionVariable>
	ConsumableEventBase<ConditionVariable>::ConsumableEventBase()
	{
		PODConsumableEventBase<ConditionVariable>::initialise();
	}

	template class PODEventBase<PODConditionVariable>;
	template class PODEventBase<PODFIFOConditionVariable>;
	template class PODConsumableEventBase<PODConditionVariable>;
	template class PODConsumableEventBase<PODFIFOConditionVariable>;

	template class EventBase<PODConditionVariable>;
	template class EventBase<PODFIFOConditionVariable>;
	template class ConsumableEventBase<PODConditionVariable>;
	template class ConsumableEventBase<PODFIFOConditionVariable>;
}