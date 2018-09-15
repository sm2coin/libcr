#include "Promise.hpp"
#include "../helpermacros.hpp"

namespace cr::sync
{
	void PODSinglePromise::initialise()
	{
		PODSingleConditionVariable::initialise();
		m_fulfilled = false;
	}

	bool PODSinglePromise::wait(
		Coroutine * self)
	{
		if(m_fulfilled)
			return true;
		else
		{
			PODSingleConditionVariable::wait(self);
			return false;
		}
	}

	void PODSinglePromise::fulfill()
	{
		assert(!m_fulfilled);
		m_fulfilled = true;

		PODSingleConditionVariable::notify_all();
	}

	SinglePromise::SinglePromise()
	{
		initialise();
	}

	void PODPromise::initialise()
	{
		PODConditionVariable::initialise();
		m_fulfilled = false;
	}

	bool PODPromise::wait(
		Coroutine * self)
	{
		if(m_fulfilled)
			return true;
		else
		{
			PODConditionVariable::wait(self);
			return false;
		}
	}

	void PODPromise::fulfill()
	{
		assert(!m_fulfilled);
		m_fulfilled = true;

		PODConditionVariable::notify_all();
	}

	Promise::Promise()
	{
		initialise();
	}
}