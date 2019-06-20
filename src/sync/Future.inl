#include "../helpermacros.hpp"

namespace cr::sync
{
	template<class Event>
	void PODFutureBasePattern<Event>::fulfill()
	{
		Event::fire();
	}

	template<class Event>
	bool PODFutureBasePattern<Event>::fulfilled() const
	{
		return Event::happened();
	}

	template<class T, class Event>
	util::copy_t<T> PODFuturePattern<T, Event>::value() const
	{
		assert(fulfilled());
		return m_value;
	}
}