#include "../helpermacros.hpp"

namespace cr::mt
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
		m_future = future;
	}

	template<class T, class Event>
	void PODPromiseBaseBase<T, Event>::initialise(
		FutureBase<T, Event> * future)
	{
		initialise(static_cast<PODFutureBase<T, Event> *>(future));
	}

	template<class Event>
	void PODPromiseBase<void, Event>::fulfill()
	{
		assert(m_future != nullptr);
		m_future->fulfill();
	}

	template<class T, class Event>
	void PODPromiseBase<T, Event>::fulfill(
		T const& value)
	{
		assert(m_future != nullptr);
		m_future->fulfill(value);
	}

	template<class T, class Event>
	void PODPromiseBase<T, Event>::fulfill(
		T && value)
	{
		assert(m_future != nullptr);
		m_future->fulfill(std::move(value));
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

	template<class T, class Event>
	PromiseBase<T, Event>::PromiseBase(
		FutureBase<T, Event> * future)
	{
		PODPromiseBase<T, Event>::initialise(future);
	}
}