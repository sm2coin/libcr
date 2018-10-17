#include "ConditionVariable.hpp"
#include "../Coroutine.hpp"

namespace cr::sync
{
	void PODConditionVariable::initialise()
	{
		m_first_waiting = nullptr;
		m_last_waiting = nullptr;
	}

	block PODConditionVariable::WaitCall::libcr_wait(
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

	void PODConditionVariable::notify_one()
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

	void PODConditionVariable::notify_all()
	{
		if(empty())
			return;

		// Only notify coroutines waiting since before notify_all().
		Coroutine * last = m_last_waiting;

		// Notify all but the last coroutine.
		while(front() != last)
			notify_one();

		// Notify the last coroutine.
		notify_one();
	}

	ConditionVariable::ConditionVariable()
	{
		initialise();
	}

	void PODSingleConditionVariable::initialise()
	{
		m_waiting = nullptr;
	}

	block PODSingleConditionVariable::WaitCall::libcr_wait(
		Coroutine * coroutine)
	{
		assert(coroutine != nullptr);
		assert(!coroutine->waiting());

		assert(m_cv.m_waiting == nullptr);

		m_cv.m_waiting = coroutine;
		coroutine->pause();

		return block();
	}

	void PODSingleConditionVariable::notify_one()
	{
		if(m_waiting)
		{
			Coroutine * first = m_waiting;
			m_waiting = nullptr;
			first->resume();
			first->directly_call_child();
		}
	}

	void PODSingleConditionVariable::notify_all()
	{
		notify_one();
	}

	SingleConditionVariable::SingleConditionVariable()
	{
		initialise();
	}
}