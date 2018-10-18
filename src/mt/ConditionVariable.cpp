#include "ConditionVariable.hpp"
#include "../Coroutine.hpp"
#include <cassert>

namespace cr::mt
{
	void PODFIFOConditionVariable::initialise()
	{
		std::atomic_init(&m_first_waiting, (Coroutine *)nullptr);
		std::atomic_init(&m_last_waiting, (Coroutine *)nullptr);
	}

	sync::block PODFIFOConditionVariable::WaitCall::libcr_wait(
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
			// `last` cannot be removed until it is successfully linked to the next coroutine, because it is no longer the last coroutine.
			assert(last->waiting());
			assert(!last->next_waiting());
			last->libcr_next_waiting.store(coroutine, std::memory_order_relaxed);
		} else
		{
			// The condition variable was previously empty, so register the condition variable as first.

			m_cv.m_first_waiting.store(coroutine, std::memory_order_relaxed);
		}

		return sync::block();
	}

	void PODFIFOConditionVariable::notify_one()
	{
		// Remove the first waiting coroutine.
		Coroutine * first = nullptr;
		while(!m_first_waiting.compare_exchange_weak(
			first,
			nullptr,
			std::memory_order_relaxed));

		// If there is no first coroutine, return.
		if(!first)
			return;

		// Now, no coroutine can be removed until first is set again.

		Coroutine * waiting = first->next_waiting();
		// Is this the only / last coroutine?
		if(!waiting)
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
				do {
					waiting = first->next_waiting();
				} while(!waiting);

				// Set the newly added coroutine as first coroutine.
				m_first_waiting.store(waiting, std::memory_order_relaxed);
			}
			// Now, the coroutine is either the only, or not the last coroutine.
			// If the coroutine is the only one, `last` has been set to null.
		} else
		{
			// Was not the last coroutine, so set the next coroutine as first coroutine.
			m_first_waiting.store(waiting, std::memory_order_relaxed);
		}

		// Resume the removed coroutine.
		first->resume();
		first->directly_call_child();
	}

	void PODFIFOConditionVariable::notify_all()
	{
		// Only notify coroutines waiting since before notify_all().

		// Remove the first coroutine.
		Coroutine * first = nullptr;
		while(!m_first_waiting.compare_exchange_weak(
			first,
			nullptr,
			std::memory_order_relaxed));
		// Now, no further coroutine can be removed while first is null.

		// 1: We need to watch out for coroutines being added before setting last to null.

		// Remove the last coroutine.
		Coroutine * last = nullptr;
		while(!m_last_waiting.compare_exchange_weak(
			last,
			nullptr,
			std::memory_order_relaxed));
		// Now, the queue is empty, and adding a coroutine sets first, unlocking the queue.

		// Notify the removed coroutines.
		Coroutine * next;
		do {
			next = first->next_waiting();
			// Is a coroutine not completely added yet (see 1)?
			if(!next && first != last)
			{
				// Wait until it is added.
				do {
					next = first->next_waiting();
				} while(!next);
			}
			// Now, next is guaranteed to be the next coroutine, or null, if first = last.
			assert(next ? first != last : first == last);

			// Notify the coroutine.
			first->resume();
			first->directly_call_child();
			// Set the net coroutine.
			first = next;
		} while(next);
	}

	FIFOConditionVariable::FIFOConditionVariable()
	{
		PODFIFOConditionVariable::initialise();
	}

	void PODConditionVariable::initialise()
	{
		std::atomic_init(&m_waiting, (Coroutine *)nullptr);
	}

	sync::block PODConditionVariable::WaitCall::libcr_wait(
		Coroutine * coroutine)
	{
		assert(coroutine != nullptr);

		// Pause the coroutine.
		coroutine->pause();

		Coroutine * first = m_cv.m_waiting.load(std::memory_order_relaxed);
		// Make the coroutine the first coroutine.
		do {
			// Set the next coroutine to be the first coroutine (if exists).
			if(first)
				coroutine->libcr_next_waiting.store(first, std::memory_order_relaxed);
			else
				coroutine->libcr_next_waiting.store(coroutine, std::memory_order_relaxed);
			// The first coroutine might have changed, so try until succeeds.
		} while(!m_cv.m_waiting.compare_exchange_weak(
			first,
			coroutine,
			std::memory_order_relaxed));

		return sync::block();
	}

	void PODConditionVariable::notify_one()
	{
		// Remove the first coroutine.
		Coroutine * removed = nullptr;
		while(!m_waiting.compare_exchange_weak(
			removed,
			nullptr,
			std::memory_order_relaxed));
		// Now, the queue is empty, so we need to add the rest back.

		// If there was no coroutine, return.
		if(!removed)
			return;

		Coroutine * removed_next = removed->next_waiting();
		// Are there coroutines to add back?
		if(removed_next)
		{
			// Remember for later.
			Coroutine * first = nullptr;
			// Try the easy way: if the queue is still empty, just add it back.
			if(!m_waiting.compare_exchange_strong(
				first,
				removed_next,
				std::memory_order_relaxed))
			{
				// Failed: We need to link the last of the removed coroutines to the first.

				// Find the last removed coroutine.
				Coroutine * tail = removed_next;
				Coroutine * tail_next;
				while(tail_next = tail->next_waiting())
					tail = tail_next;

				// Try to add the last removed coroutine to the queue.
				first = m_waiting.load(std::memory_order_relaxed);
				// Make the second removed coroutine the first coroutine.
				do {
					// Set the next coroutine to be the first coroutine (if exists).
					if(first)
						tail->libcr_next_waiting.store(first, std::memory_order_relaxed);
					else
						tail->libcr_next_waiting.store(tail, std::memory_order_relaxed);
					// The first coroutine might have changed, so try until succeeds.
				} while(!m_waiting.compare_exchange_weak(
					first,
					removed_next,
					std::memory_order_relaxed));
			}
		}

		// Notify the first removed coroutine.
		removed->resume();
		removed->directly_call_child();
	}

	void PODConditionVariable::notify_all()
	{
		// Remove the first coroutine.
		Coroutine * removed = nullptr;
		while(!m_waiting.compare_exchange_weak(
			removed,
			nullptr,
			std::memory_order_relaxed));
		// Now, the queue is empty.

		// If there was no coroutine, return.
		if(!removed)
			return;

		// Notify all removed coroutines.
		Coroutine * next;
		do {
			next = removed->next_waiting();
			removed->resume();
			removed->directly_call_child();
			removed = next;
		} while(next);
	}

	ConditionVariable::ConditionVariable()
	{
		PODConditionVariable::initialise();
	}
}