#include "ConditionVariable.hpp"
#include "../Coroutine.hpp"
#include "../helpermacros.hpp"

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

		coroutine->libcr_next_waiting.atomic.release();

		// Make the coroutine the last coroutine.
		// Release ordering because this must not be reordered before the releasing of the coroutine.
		Coroutine * last = m_cv.m_last_waiting.exchange(
			coroutine,
			std::memory_order_release);

		// `last` is now the previously last coroutine.
		// `last` can only be null for the coroutine that comes first.
		if(last)
		{
			// The condition variable was not empty previously, so link the new condition variable to the previously last coroutine.
			// `last` cannot be removed until it is successfully linked to the next coroutine, because it is no longer the last coroutine.

			// Set the coroutine to be the next waiting of last.
			last->libcr_next_waiting.atomic.update(coroutine);
		} else
		{
			// The condition variable was previously empty, so register the condition variable as first.
			assert(!m_cv.m_first_waiting.load(std::memory_order_relaxed));

			// Relaxed is sufficient, as no reordering can occur.
			m_cv.m_first_waiting.store(coroutine, std::memory_order_relaxed);
		}

		return sync::block();
	}

	bool PODFIFOConditionVariable::notify_one()
	{
		Coroutine * removed = remove_one();

		if(removed)
		{
			(*removed)();
			return true;
		} else {
			return false;
		}
	}

	bool PODFIFOConditionVariable::fail_one()
	{
		Coroutine * removed = remove_one();

		if(removed)
		{
			removed->libcr_error = true;
			(*removed)();
			return true;
		} else {
			return false;
		}
	}

	Coroutine * PODFIFOConditionVariable::remove_one()
	{
		// Remove the first coroutine.
		Coroutine * first = m_first_waiting.exchange(
			nullptr,
			std::memory_order_relaxed);

		if(!first)
			return nullptr;

		// No other coroutines can be removed until first is set again.

		// Acquire the coroutine's state and get its successor.
		Coroutine * next = first->libcr_next_waiting.atomic.acquire_strong();

		if(!next)
		{
			// We only had a single coroutine.
			Coroutine * expect = first;
			// Clear the last waiting coroutine, but only if no new coroutine arrived.
			if(m_last_waiting.compare_exchange_strong(
				expect,
				nullptr,
				std::memory_order_relaxed))
			{
				return first;
			} else
			{
				// There was a new coroutine added.
				// Wait until it sets this coroutine's next pointer.
				next = first->libcr_next_waiting.atomic.wait_weak();
			}
		}

		assert(!m_first_waiting.load(std::memory_order_relaxed));
		// We have a successor, set it as first.
		m_first_waiting.store(next, std::memory_order_relaxed);

		// The coroutine was already resumed earlier, return the coroutine.
		return first;
	}

	bool PODFIFOConditionVariable::notify_all()
	{
		Coroutine * first, * last;
		// Return if there are no coroutines.
		if(!remove_all(first, last))
			return false;

		// Notify the removed coroutines.
		Coroutine * next;
		do {
			next = acquire_and_complete(first, last);

			// Notify the coroutine.
			(*first)();
			// Set the next coroutine.
			first = next;
		} while(next);

		return true;
	}

	bool PODFIFOConditionVariable::fail_all()
	{
		Coroutine * first, * last;
		// Return if there are no coroutines.
		if(!remove_all(first, last))
			return false;

		// Notify the removed coroutines.
		Coroutine * next;
		do {
			next = acquire_and_complete(first, last);

			first->libcr_error = true;
			// Notify the coroutine.
			(*first)();
			// Set the next coroutine.
			first = next;
		} while(next);

		return true;
	}

	bool PODFIFOConditionVariable::remove_all(
		Coroutine * &first,
		Coroutine * &last)
	{
		// Only notify coroutines waiting since before notify_all().

		// Remove the first coroutine.
		first = m_first_waiting.exchange(
			nullptr,
			std::memory_order_relaxed);
		// Now, no further coroutine can be removed while first is null.

		// If there was no first coroutine, then there is nothing to notify.
		if(!first)
		{
			last = nullptr;
			return false;
		}

		// We need to watch out for coroutines being added before setting last to null.

		// Remove the last coroutine.
		last = m_last_waiting.exchange(
			nullptr,
			std::memory_order_relaxed);
		// Now, the queue is empty, and adding a coroutine sets first, unlocking the queue.

		// Sanity check: The last coroutine should not be null.
		assert(last != nullptr);

		return true;
	}

	Coroutine * PODFIFOConditionVariable::acquire_and_complete(
		Coroutine * coroutine,
		Coroutine * last)
	{
		// Does this coroutine have a successor?
		if(coroutine != last)
		{
			// Acquire the coroutine.
			Coroutine * next = coroutine->libcr_next_waiting.atomic.acquire_strong();
			if(!next)
				// Wait until the coroutine has a next in line.
				next = coroutine->libcr_next_waiting.atomic.wait_weak();
			// Return the next coroutine.
			return next;
		} else
		{
			// Acquire the coroutine, ignore the next in line.
			coroutine->libcr_next_waiting.atomic.acquire_strong();
			return nullptr;
		}

	}

	FIFOConditionVariable::FIFOConditionVariable()
	{
		initialise();
	}

	FIFOConditionVariable::~FIFOConditionVariable()
	{
		fail_all();
		assert(!remove_one() && "Coroutines were added illegally.");
	}

	void PODConditionVariable::initialise()
	{
		std::atomic_init(&m_waiting, (Coroutine *)nullptr);
	}

	sync::block PODConditionVariable::WaitCall::libcr_wait(
		Coroutine * coroutine)
	{
		assert(coroutine != nullptr);

		// Weak load sufficient, as the loop will perform a strong load anyway if necessary.
		Coroutine * first = m_cv.m_waiting.load(std::memory_order_relaxed);
		// Make the coroutine the first coroutine.
		do {
			// Release the coroutine and set its successor to be the first waiting coroutine.
			if(first)
				coroutine->libcr_next_waiting.atomic.release_with_next(first);
			else
				coroutine->libcr_next_waiting.atomic.release();
			// The first coroutine might have changed, so try until succeeds.
		} while(!m_cv.m_waiting.compare_exchange_weak(
			first,
			coroutine,
			std::memory_order_relaxed));

		return sync::block();
	}

	bool PODConditionVariable::notify_one()
	{
		Coroutine * removed = remove_one();
		if(removed)
		{
			// Notify the first removed coroutine.
			(*removed)();

			return true;
		} else
			return false;
	}

	bool PODConditionVariable::fail_one()
	{
		Coroutine * removed = remove_one();
		if(removed)
		{
			removed->libcr_error = true;
			// Notify the first removed coroutine.
			(*removed)();

			return true;
		} else
			return false;
	}

	Coroutine * PODConditionVariable::remove_one()
	{
		// Remove the first coroutine.
		Coroutine * removed = m_waiting.exchange(
			nullptr,
			std::memory_order_relaxed);
		// Now, the queue is empty, so we need to add the rest back.

		// If there was no coroutine, return.
		if(!removed)
			return nullptr;

		// Acquire the coroutine and get next waiting coroutine.
		Coroutine * removed_next = removed->libcr_next_waiting.atomic.acquire_strong();

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
				for(;;)
				{
					// Get the next coroutine.
					tail_next = tail->libcr_next_waiting.atomic.load_strong(
						std::memory_order_relaxed);

					assert(tail_next != nullptr);
					// Is there a next coroutine waiting?
					if(tail_next != tail)
						tail = tail_next;
					else break;
				}

				// Try to add the last removed coroutine to the front of the queue.
				do {
					// Set the first queued coroutine to be the next coroutine.
					tail->libcr_next_waiting.atomic.update(first);
					// The first coroutine might have changed, so try until succeeds.
				} while(!m_waiting.compare_exchange_weak(
					first,
					removed_next,
					std::memory_order_relaxed));
			}
		}

		return removed;
	}

	bool PODConditionVariable::notify_all()
	{
		// Remove the waiting coroutines.
		Coroutine * first;
		Coroutine * last;
		if(!remove_all(first, last))
		// If there was no coroutine, return.
			return false;

		// Now, the queue is empty.

		// Notify all removed coroutines.
		Coroutine * next;
		do {
			// Resume the coroutine and get the next in line.
			next = acquire_and_complete(first, nullptr);
			// notify the coroutine.
			(*first)();

			first = next;
		} while(next);
		return true;
	}

	bool PODConditionVariable::fail_all()
	{
		// Remove the waiting coroutines.
		Coroutine * first;
		Coroutine * last;
		if(!remove_all(first, last))
		// If there was no coroutine, return.
			return false;

		// Now, the queue is empty.

		// Notify all removed coroutines.
		Coroutine * next;
		do {
			// Resume the coroutine and get the next in line.
			next = acquire_and_complete(first, nullptr);
			first->libcr_error = true;
			// notify the coroutine.
			(*first)();

			first = next;
		} while(next);
		return true;
	}

	bool PODConditionVariable::remove_all(
		Coroutine * &first,
		Coroutine * &last)
	{
		first = m_waiting.exchange(
			nullptr,
			std::memory_order_relaxed);

		if(!first)
			return false;

		last = nullptr;
		return true;
	}

	Coroutine * PODConditionVariable::acquire_and_complete(
		Coroutine * coroutine,
		Coroutine *)
	{
		assert(coroutine != nullptr);
		return coroutine->libcr_next_waiting.atomic.acquire_strong();
	}

	ConditionVariable::ConditionVariable()
	{
		initialise();
	}

	ConditionVariable::~ConditionVariable()
	{
		fail_all();
		assert(!remove_one() && "Coroutines were added illegally.");
	}
}