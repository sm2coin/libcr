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

	bool PODFIFOConditionVariable::notify_one()
	{
		if(empty())
			return false;

		Coroutine * first = m_first_waiting;

		if(m_first_waiting == m_last_waiting)
		{
			assert(!m_first_waiting->next_waiting());
			m_last_waiting = nullptr;
		}

		m_first_waiting = first->next_waiting();

		first->resume();
		(*first)();

		return true;
	}

	bool PODFIFOConditionVariable::fail_one()
	{
		if(empty())
			return false;

		Coroutine * first = m_first_waiting;

		if(m_first_waiting == m_last_waiting)
		{
			assert(!m_first_waiting->next_waiting());
			m_last_waiting = nullptr;
		}

		m_first_waiting = first->next_waiting();

		first->resume();
		first->libcr_error = true;
		(*first)();

		return true;
	}

	bool PODFIFOConditionVariable::notify_all()
	{
		Coroutine * coroutine = m_first_waiting;
		m_first_waiting = nullptr;
		m_last_waiting = nullptr;

		if(!coroutine)
			return false;

		while(coroutine)
		{
			Coroutine * next = coroutine->next_waiting();

			coroutine->resume();
			(*coroutine)();

			coroutine = next;
		}

		return true;
	}

	bool PODFIFOConditionVariable::fail_all()
	{
		Coroutine * coroutine = m_first_waiting;
		m_first_waiting = nullptr;
		m_last_waiting = nullptr;

		if(!coroutine)
			return false;

		while(coroutine)
		{
			Coroutine * next = coroutine->next_waiting();

			coroutine->resume();
			coroutine->libcr_error = true;
			(*coroutine)();

			coroutine = next;
		}

		return true;
	}

	FIFOConditionVariable::FIFOConditionVariable()
	{
		initialise();
	}

	FIFOConditionVariable::~FIFOConditionVariable()
	{
		fail_all();
		assert(!front() && "Coroutines were added illegally.");
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

	bool PODConditionVariable::notify_one()
	{
		if(!m_waiting)
			return false;

		Coroutine * first = m_waiting;
		m_waiting = first->next_waiting();
		first->resume();
		(*first)();

		return true;
	}

	bool PODConditionVariable::fail_one()
	{
		if(!m_waiting)
			return false;

		Coroutine * first = m_waiting;
		m_waiting = first->next_waiting();
		first->resume();
		first->libcr_error = true;
		(*first)();

		return true;
	}

	bool PODConditionVariable::notify_all()
	{
		Coroutine * coroutine = m_waiting;
		m_waiting = nullptr;

		if(!coroutine)
			return false;

		while(coroutine)
		{
			Coroutine * next = coroutine->next_waiting();

			coroutine->resume();
			(*coroutine)();

			coroutine = next;
		}

		return true;
	}

	bool PODConditionVariable::fail_all()
	{
		Coroutine * coroutine = m_waiting;
		m_waiting = nullptr;

		if(!coroutine)
			return false;

		while(coroutine)
		{
			Coroutine * next = coroutine->next_waiting();

			coroutine->resume();
			coroutine->libcr_error = true;
			(*coroutine)();

			coroutine = next;
		}

		return true;
	}

	ConditionVariable::ConditionVariable()
	{
		initialise();
	}

	ConditionVariable::~ConditionVariable()
	{
		fail_all();
		assert(!front() && "Coroutines were added illegally.");
	}
}