#include "../helpermacros.hpp"

namespace cr::sync
{
	template<class T, class Event>
	void PODPromiseBaseBase<T, Event>::initialise()
	{
		m_future = nullptr;
	}

	template<class T, class Event>
	void PODPromiseBaseBase<T, Event>::initialise(
		PODFutureBase<T, Event> * future)
	{
		assert(future != nullptr);
		assert(!future->waiting());
		m_future = future;
	}

	template<class Event>
	void PODPromiseBase<void, Event>::fulfill()
	{
		initialise();
		assert(m_future != nullptr);
		m_future->fulfill();
	}

	template<class T, class Event>
	void PODPromiseBase<T, Event>::fulfill(
		T const& value)
	{
		assert(m_future);
		m_future->m_value = value;
		m_future->fulfill();
	}

	template<class T, class Event>
	void PODPromiseBase<T, Event>::fulfill(
		T && value)
	{
		assert(m_future);
		m_future->m_value = std::move(value);
		m_future->fulfill();
	}

	template<class T, class Event>
	PromiseBase<T, Event>::PromiseBase()
	{
		PODPromiseBase<T, Event>::initialise();
	}

	template<class T, class Event>
	PromiseBase<T, Event>::PromiseBase(
		PODFutureBase<T, Event> * future)
	{
		PODPromiseBase<T, Event>::initialise(future);
	}
}