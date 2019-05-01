/** @file Future.hpp
	Contains the future class. */
#ifndef __libcr_sync_future_hpp_defined
#define __libcr_sync_future_hpp_defined

#include "Event.hpp"
#include "../util/Argument.hpp"

namespace cr::sync
{
	template<class T, class Event>
	class PODPromisePattern;
	template<class T, class Event>
	class PODPromiseBasePattern;

	template<class Event>
	/** POD future base class.
		Essentially a non-repeatable event.
	@tparam Event:
		The event flavour to use. */
	class PODFutureBasePattern : Event
	{
	public:
		using Event::wait;
		using Event::initialise;

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
	class PODFuturePattern;

	template<class Event>
	/** POD class for futures without a value.
	@tparam Event:
		The future's event flavour. */
	class PODFuturePattern<void, Event> : PODFutureBasePattern<Event>
	{
		friend class PODPromisePattern<void, Event>;

		using PODFutureBasePattern<Event>::fulfill;
	public:
		using PODFutureBasePattern<Event>::wait;
		using PODFutureBasePattern<Event>::initialise;
		using PODFutureBasePattern<Event>::fulfilled;
		using PODFutureBasePattern<Event>::listeners;

		/** The future's value type. */
		typedef void value_t;
	};

	template<class T, class Event>
	class PODFuturePattern : PODFutureBasePattern<Event>
	{
		friend class PODPromisePattern<T, Event>;
		/** The future's value. */
		T m_value;

		using PODFutureBasePattern<Event>::fulfill;
	public:
		using PODFutureBasePattern<Event>::wait;
		using PODFutureBasePattern<Event>::initialise;
		using PODFutureBasePattern<Event>::fulfilled;
		using PODFutureBasePattern<Event>::listeners;

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
	class FuturePattern;

	template<class Event>
	/** Future class without a value.
	@tparam Event:
		The event flavour to use. */
	class FuturePattern<void, Event> : PODFuturePattern<void, Event>
	{
		friend class PODPromisePattern<void, Event>;
		friend class PODPromiseBasePattern<void, Event>;
	public:
		/** Initialises the future to an unfulfilled state. */
		FuturePattern();

		using PODFuturePattern<void, Event>::wait;
		using PODFuturePattern<void, Event>::initialise;
		using PODFuturePattern<void, Event>::fulfilled;
		using PODFuturePattern<void, Event>::listeners;

		using typename PODFuturePattern<void, Event>::value_t;
	};

	template<class T, class Event>
	class FuturePattern : PODFuturePattern<T, Event>
	{
		friend class PODPromisePattern<T, Event>;
		friend class PODPromiseBasePattern<T, Event>;
	public:
		/** Initialises the future to an unfulfilled state. */
		FuturePattern();

		using PODFuturePattern<T, Event>::wait;
		using PODFuturePattern<T, Event>::initialise;
		using PODFuturePattern<T, Event>::fulfilled;
		using PODFuturePattern<T, Event>::listeners;
		using PODFuturePattern<T, Event>::value;

		using typename PODFuturePattern<T, Event>::value_t;
	};


	template<class T>
	/** POD future type. */
	using PODFuture = PODFuturePattern<T, PODEvent>;
	template<class T>
	/** Future type. */
	using Future = FuturePattern<T, PODEvent>;

	template<class T>
	/** POD future type that supports only one waiting coroutine. */
	using PODFIFOFuture = PODFuturePattern<T, PODFIFOEvent>;
	template<class T>
	/** Future type that supports only one waiting coroutine. */
	using FIFOFuture = FuturePattern<T, PODFIFOEvent>;
}

#include "Future.inl"

#endif