#include "ConditionVariable.hpp"
#include "../Coroutine.hpp"

namespace cr::sync
{
	void PODConditionVariable::initialise()
	{
		m_first_waiting = nullptr;
		m_last_waiting = nullptr;
	}

	void PODConditionVariable::wait(
		Coroutine * coroutine)
	{
		assert(coroutine != nullptr);
		assert(coroutine->libcr_next_waiting == nullptr);

		coroutine->libcr_next_waiting = coroutine;

		if(!m_first_waiting)
		{
			m_first_waiting = coroutine;
			m_last_waiting = coroutine;
		} else
		{
			assert(m_last_waiting->libcr_next_waiting == m_last_waiting);

			m_last_waiting->libcr_next_waiting = coroutine;
			m_last_waiting = coroutine;
		}

	}

	void PODConditionVariable::notify_one()
	{
		if(empty())
			return;

		assert(m_first_waiting->libcr_next_waiting != nullptr);

		Coroutine * first = m_first_waiting;

		if(m_first_waiting == m_last_waiting)
		{
			assert(m_first_waiting->libcr_next_waiting == m_first_waiting);

			m_first_waiting->libcr_next_waiting = nullptr;
			m_first_waiting = nullptr;
			m_last_waiting = nullptr;
		} else
		{
			Coroutine * second = m_first_waiting->libcr_next_waiting;
			m_first_waiting->libcr_next_waiting = nullptr;
			m_first_waiting = second;
		}

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

	void PODSingleConditionVariable::wait(
		Coroutine * coroutine)
	{
		assert(m_waiting == nullptr);
		assert(coroutine->libcr_next_waiting == nullptr);

		m_waiting = coroutine;
		coroutine->libcr_next_waiting = coroutine;
	}

	void PODSingleConditionVariable::notify_one()
	{
		if(m_waiting)
		{
			Coroutine * first = front();
			m_waiting->libcr_next_waiting = nullptr;
			m_waiting = nullptr;
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