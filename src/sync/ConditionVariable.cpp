#include "ConditionVariable.hpp"
#include "../Coroutine.hpp"

#include "Barrier.hpp"

namespace cr::sync
{
	void PODFIFOConditionVariable::initialise()
	{
		m_first_waiting = nullptr;
		m_last_waiting = nullptr;
	}

	block PODFIFOConditionVariable::WaitCall::libcr_wait(
		Coroutine * coroutine)
	{
		assert(coroutine != nullptr);
		assert(!coroutine->waiting());

		coroutine->pause();

		if(!m_cv.m_first_waiting)
		{
			m_cv.m_first_waiting = coroutine;
		} else
		{
			m_cv.m_last_waiting->libcr_next_waiting = coroutine;
		}

		m_cv.m_last_waiting = coroutine;

		return block();
	}

	void PODFIFOConditionVariable::notify_one()
	{
		if(empty())
			return;

		Coroutine * first = m_first_waiting;

		if(m_first_waiting == m_last_waiting)
		{
			assert(!m_first_waiting->next_waiting());
			m_last_waiting = nullptr;
		}

		m_first_waiting = first->next_waiting();

		first->resume();
		first->directly_call_child();
	}

	void PODFIFOConditionVariable::notify_all()
	{
		Coroutine * coroutine = m_first_waiting;
		m_first_waiting = nullptr;
		m_last_waiting = nullptr;
		while(coroutine)
		{
			Coroutine * next = coroutine->next_waiting();

			coroutine->resume();
			coroutine->directly_call_child();

			coroutine = next;
		}
	}

	FIFOConditionVariable::FIFOConditionVariable()
	{
		initialise();
	}

	void PODConditionVariable::initialise()
	{
		m_waiting = nullptr;
	}

	block PODConditionVariable::WaitCall::libcr_wait(
		Coroutine * coroutine)
	{
		assert(coroutine != nullptr);
		assert(!coroutine->waiting());

		if(m_cv.m_waiting)
			coroutine->libcr_next_waiting.store(m_cv.m_waiting, std::memory_order_relaxed);
		else
			coroutine->pause();

		m_cv.m_waiting = coroutine;

		return block();
	}

	void PODConditionVariable::notify_one()
	{
		if(m_waiting)
		{
			Coroutine * first = m_waiting;
			m_waiting = first->next_waiting();
			first->resume();
			first->directly_call_child();
		}
	}

	void PODConditionVariable::notify_all()
	{
		Coroutine * coroutine = m_waiting;
		m_waiting = nullptr;
		while(coroutine)
		{
			Coroutine * next = coroutine->next_waiting();

			coroutine->resume();
			coroutine->directly_call_child();

			coroutine = next;
		}
	}

	ConditionVariable::ConditionVariable()
	{
		initialise();
	}
}