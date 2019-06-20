#include "../helpermacros.hpp"

namespace cr::sync
{
	template<class T, class Event>
	void PODPromiseBasePattern<T, Event>::initialise()
	{
		m_future = nullptr;
	}

	template<class T, class Event>
	void PODPromiseBasePattern<T, Event>::initialise(
		PODFuturePattern<T, Event> * future)
	{
		assert(future != nullptr);
		m_future = future;
	}

	template<class T, class Event>
	void PODPromiseBasePattern<T, Event>::initialise(
		FuturePattern<T, Event> * future)
	{
		initialise(static_cast<PODFuturePattern<T, Event> *>(future));
	}

	template<class Event>
	void PODPromisePattern<void, Event>::fulfill()
	{
		initialise();
		assert(m_future != nullptr);
		m_future->fulfill();
	}

	template<class T, class Event>
	void PODPromisePattern<T, Event>::fulfill(
		T const& value)
	{
		assert(m_future);
		m_future->m_value = value;
		m_future->fulfill();
	}

	template<class T, class Event>
	void PODPromisePattern<T, Event>::fulfill(
		T && value)
	{
		assert(m_future);
		m_future->m_value = std::move(value);
		m_future->fulfill();
	}

	template<class T, class Event>
	PromisePattern<T, Event>::PromisePattern()
	{
		PODPromisePattern<T, Event>::initialise();
	}

	template<class T, class Event>
	PromisePattern<T, Event>::PromisePattern(
		PODFuturePattern<T, Event> * future)
	{
		PODPromisePattern<T, Event>::initialise(future);
	}

	template<class T, class Event>
	PromisePattern<T, Event>::PromisePattern(
		FuturePattern<T, Event> * future)
	{
		PODPromisePattern<T, Event>::initialise(future);
	}
}