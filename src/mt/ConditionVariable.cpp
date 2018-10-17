#include "ConditionVariable.hpp"
#include "../Coroutine.hpp"
#include <cassert>

namespace cr::mt
{
	void PODConditionVariable::initialise()
	{
		m_first_waiting.store(nullptr, std::memory_order_relaxed);
		m_last_waiting.store(nullptr, std::memory_order_relaxed);
	}

	sync::block PODConditionVariable::WaitCall::libcr_wait(
		Coroutine * coroutine)
	{
		assert(coroutine != nullptr);

		// Pause the coroutine.
		coroutine->pause();

		Coroutine * last = nullptr;
		// Make the coroutine the last coroutine.
		while(!m_cv.m_last_waiting.compare_exchange_weak(
			last,
			coroutine,
			std::memory_order_relaxed));

		// `last` is now the previously last coroutine.
		if(last)
		{
			assert(last->waiting());
			assert(!last->next_waiting());
			last->libcr_next_waiting.store(coroutine, std::memory_order_relaxed);
		} else
		{
			// The condition variable must be empty, so set last as first.
				Coroutine * first;
			do {
				first = nullptr;
			// If first is not null, the previously only coroutine is being notified, wait.
			} while(!m_cv.m_first_waiting.compare_exchange_weak(
				first,
				coroutine,
				std::memory_order_relaxed));
		}

		return sync::block();
	}

	void PODConditionVariable::notify_one()
	{
		// Remove the first waiting coroutine.
		Coroutine * first = m_first_waiting.load(std::memory_order_relaxed);
		do {
			// If there is no first coroutine, return.
			if(!first)
				return;

			// Is this the only / last coroutine?
			if(!first->next_waiting())
			{
				// Dummy to keep `first` if exchange fails.
				Coroutine * last = first;
				// If it is the only coroutine, set last to null before removing it.
				// This requires strong exchange.
				if(!m_last_waiting.compare_exchange_strong(
					last,
					nullptr,
					std::memory_order_relaxed))
				{
					// A coroutine is currently being added to the first coroutine, wait.
					while(!first->next_waiting());
				}
				// Now, the coroutine is either the only, or not the last coroutine.
				// If the coroutine is the only one, `last` has been set to null.
			}
			// Try to remove the first waiting coroutine.
		} while(!m_first_waiting.compare_exchange_weak(
			first,
			first->next_waiting(),
			std::memory_order_relaxed));

		// Resume the removed coroutine.
		first->resume();
		first->directly_call_child();
	}

	void PODConditionVariable::notify_all()
	{
		// Only notify coroutines waiting since before notify_all().
		// Remember the last coroutine to notify.
		Coroutine * last = m_last_waiting.load(std::memory_order_relaxed);

		// Notify all until the `last` is reached, or none left.
		while(!empty())
		{
			bool is_last = (m_first_waiting.load(std::memory_order_relaxed) == last);
			notify_one();

			if(is_last)
				return;
		}
	}

	ConditionVariable::ConditionVariable()
	{
		PODConditionVariable::initialise();
	}
}