#include "../helpermacros.hpp"

namespace cr::sync
{
	template<class Event>
	void PODFutureBaseBase<Event>::fulfill()
	{
		Event::fire();
	}

	template<class Event>
	bool PODFutureBaseBase<Event>::fulfilled() const
	{
		return Event::happened();
	}

	template<class T, class Event>
	util::copy_t<T> PODFutureBase<T, Event>::value() const
	{
		assert(fulfilled());
		return m_value;
	}
}