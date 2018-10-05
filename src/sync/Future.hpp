/** @file Future.hpp
	Contains the future class. */
#ifndef __libcr_sync_future_hpp_defined
#define __libcr_sync_future_hpp_defined

#include "Event.hpp"
#include "../util/Argument.hpp"

namespace cr::sync
{
	template<class T, class Event>
	class PODPromiseBase;
	template<class T, class Event>
	class PODPromiseBaseBase;

	template<class Event>
	/** POD future base class.
		Essentially a non-repeatable event.
	@tparam Event:
		The event flavour to use. */
	class PODFutureBaseBase : Event
	{
	public:
		using Event::wait;
		using Event::initialise;
		using Event::listeners;

		/** Fulfills the promise.
			The promise must not yet be fulfilled. All waiting coroutines are notified. */
		void fulfill();
		/** Whether the promise was fulfilled. */
		inline bool fulfilled() const;
	};

	template<class T, class Event>
	/** POD class for futures with a value.
	@tparam T:
		The future's value type.
	@tparam Event:
		The future's event flavour. */
	class PODFutureBase;

	template<class Event>
	/** POD class for futures without a value.
	@tparam Event:
		The future's event flavour. */
	class PODFutureBase<void, Event> : PODFutureBaseBase<Event>
	{
		friend class PODPromiseBase<void, Event>;

		using PODFutureBaseBase<Event>::fulfill;
	public:
		using PODFutureBaseBase<Event>::wait;
		using PODFutureBaseBase<Event>::initialise;
		using PODFutureBaseBase<Event>::fulfilled;
		using PODFutureBaseBase<Event>::listeners;

		/** The future's value type. */
		typedef void value_t;
	};

	template<class T, class Event>
	class PODFutureBase : PODFutureBaseBase<Event>
	{
		friend class PODPromiseBase<T, Event>;
		/** The future's value. */
		T m_value;

		using PODFutureBaseBase<Event>::fulfill;
	public:
		using PODFutureBaseBase<Event>::wait;
		using PODFutureBaseBase<Event>::initialise;
		using PODFutureBaseBase<Event>::fulfilled;
		using PODFutureBaseBase<Event>::listeners;

		/** Retrieves the future's value.
			The future must be fulfilled.
		@return
			A copy of the future's value. */
		inline util::copy_t<T> value() const;

		/** The future's value type. */
		typedef T value_t;
	};

	template<class T, class Event>
	/** Non-POD future class with a value.
	@tparam T:
		The future's value type.
	@tparam Event:
		The event flavour to use. */
	class FutureBase;

	template<class Event>
	/** Future class without a value.
	@tparam Event:
		The event flavour to use. */
	class FutureBase<void, Event> : PODFutureBase<void, Event>
	{
		friend class PODPromiseBase<void, Event>;
		friend class PODPromiseBaseBase<void, Event>;
	public:
		/** Initialises the future to an unfulfilled state. */
		FutureBase();

		using PODFutureBase<void, Event>::wait;
		using PODFutureBase<void, Event>::initialise;
		using PODFutureBase<void, Event>::fulfilled;
		using PODFutureBase<void, Event>::listeners;

		using typename PODFutureBase<void, Event>::value_t;
	};

	template<class T, class Event>
	class FutureBase : PODFutureBase<T, Event>
	{
		friend class PODPromiseBase<T, Event>;
		friend class PODPromiseBaseBase<T, Event>;
	public:
		/** Initialises the future to an unfulfilled state. */
		FutureBase();

		using PODFutureBase<T, Event>::wait;
		using PODFutureBase<T, Event>::initialise;
		using PODFutureBase<T, Event>::fulfilled;
		using PODFutureBase<T, Event>::listeners;
		using PODFutureBase<T, Event>::value;

		using typename PODFutureBase<T, Event>::value_t;
	};


	template<class T>
	/** POD future type. */
	using PODFuture = PODFutureBase<T, PODEvent>;
	template<class T>
	/** Future type. */
	using Future = FutureBase<T, PODEvent>;

	template<class T>
	/** POD future type that supports only one waiting coroutine. */
	using PODSingleFuture = PODFutureBase<T, PODSingleEvent>;
	template<class T>
	/** Future type that supports only one waiting coroutine. */
	using SingleFuture = FutureBase<T, PODSingleEvent>;
}

#include "Future.inl"

#endif