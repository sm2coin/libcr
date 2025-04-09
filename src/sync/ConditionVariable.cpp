#ifndef __libcr_sync_conditionvariable_cpp_defined
#define __libcr_sync_conditionvariable_cpp_defined

#ifndef LIBCR_SYNC_CONDITIONVARIABLE_INLINE
#include "ConditionVariable.hpp"
#else
#undef LIBCR_SYNC_CONDITIONVARIABLE_INLINE
#endif
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

		coroutine->libcr_next_waiting.plain = nullptr;

		if(!m_cv.m_first_waiting)
		{
			m_cv.m_first_waiting = coroutine;
		} else
		{
			m_cv.m_last_waiting->libcr_next_waiting.plain = coroutine;
		}

		m_cv.m_last_waiting = coroutine;

		return block();
	}

	bool PODFIFOConditionVariable::notify_one()
	{
		Coroutine * first = m_first_waiting;
		if(first)
		{
			if(m_first_waiting == m_last_waiting)
			{
				assert(!m_last_waiting->libcr_next_waiting.plain);
				m_last_waiting = nullptr;
			}

			m_first_waiting = first->libcr_next_waiting.plain;

			(*first)();
		}

		return first != nullptr;
	}

	bool PODFIFOConditionVariable::fail_one()
	{
		Coroutine * first = m_first_waiting;
		if(first)
		{
			if(m_first_waiting == m_last_waiting)
			{
				assert(!m_last_waiting->libcr_next_waiting.plain);
				m_last_waiting = nullptr;
			}

			m_first_waiting = first->libcr_next_waiting.plain;

			first->libcr_error = true;
			(*first)();
		}

		return first != nullptr;
	}

	Coroutine * PODFIFOConditionVariable::remove_one()
	{
		Coroutine * first = m_first_waiting;

		if(!first)
			return nullptr;

		if(m_first_waiting == m_last_waiting)
		{
			assert(!m_last_waiting->libcr_next_waiting.plain);
			m_last_waiting = nullptr;
		}

		m_first_waiting = first->libcr_next_waiting.plain;

		return first;
	}

	bool PODFIFOConditionVariable::notify_all()
	{
		Coroutine * coroutine = m_first_waiting;

		if(!coroutine)
			return false;

		m_first_waiting = m_last_waiting = nullptr;

		Coroutine * next;
		do
		{
			next = coroutine->libcr_next_waiting.plain;

			(*coroutine)();
		} while((coroutine = next));

		return true;
	}

	bool PODFIFOConditionVariable::fail_all()
	{
		Coroutine * coroutine = m_first_waiting;

		if(!coroutine)
			return false;

		m_first_waiting = m_last_waiting = nullptr;

		Coroutine * next;
		do
		{
			next = coroutine->libcr_next_waiting.plain;

			coroutine->libcr_error = true;
			(*coroutine)();

		} while((coroutine = next));

		return true;
	}

	Coroutine * PODFIFOConditionVariable::remove_all()
	{
		Coroutine * first = m_first_waiting;
		if(first)
		{
			m_first_waiting = nullptr;
			m_last_waiting = nullptr;
		}
		return first;
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

		coroutine->libcr_next_waiting.plain = m_cv.m_waiting;
		m_cv.m_waiting = coroutine;

		return block();
	}

	bool PODConditionVariable::notify_one()
	{
		if(!m_waiting)
			return false;

		Coroutine * first = m_waiting;
		m_waiting = first->libcr_next_waiting.plain;
		(*first)();

		return true;
	}

	bool PODConditionVariable::fail_one()
	{
		if(!m_waiting)
			return false;

		Coroutine * first = m_waiting;
		m_waiting = first->libcr_next_waiting.plain;

		first->libcr_error = true;
		(*first)();

		return true;
	}

	Coroutine * PODConditionVariable::remove_one()
	{
		Coroutine * first = m_waiting;
		if(first)
			m_waiting = first->libcr_next_waiting.plain;
		return first;
	}

	bool PODConditionVariable::notify_all()
	{
		Coroutine * coroutine = m_waiting;
		m_waiting = nullptr;

		if(!coroutine)
			return false;

		while(coroutine)
		{
			Coroutine * next = coroutine->libcr_next_waiting.plain;

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
			Coroutine * next = coroutine->libcr_next_waiting.plain;

			coroutine->libcr_error = true;
			(*coroutine)();

			coroutine = next;
		}

		return true;
	}

	Coroutine * PODConditionVariable::remove_all()
	{
		Coroutine * first = m_waiting;
		if(first)
			m_waiting = nullptr;
		return first;

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

#endif