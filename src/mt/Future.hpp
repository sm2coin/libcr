/** @file Future.hpp
	Contains the thread-safe future type. */
#ifndef __libcr_mt_future_hpp_defined
#define __libcr_mt_future_hpp_defined

#include "Event.hpp"
#include "../util/Argument.hpp"

namespace cr::mt
{
	template<class T, class Event>
	class PODPromisePattern;
	template<class Event>
	class PODFutureBasePattern;
	template<class T, class Event>
	class PODPromiseBasePattern;

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
	class PODFuturePattern<void, Event> : Event
	{
		friend class PODPromisePattern<void, Event>;
		/** Fulfills the promise. */
		inline void fulfill();
	public:
		using Event::wait;
		using Event::initialise;
		/** Whether the future is fulfilled. */
		inline bool fulfilled() const;

		/** The future's value type. */
		typedef void value_t;
	};

	template<class T, class Event>
	class PODFuturePattern : Event
	{
		friend class PODPromisePattern<T, Event>;
		/** The future's value. */
		T m_value;
		/** To avoid race conditions when fulfilling the promise. */
		detail::PODSoftMutex m_mutex;

		/** Fulfills the promise (move assignment). */
		bool fulfill(
			T && value);
		/** Fulfills the promise (copy assignment). */
		bool fulfill(
			T const& value);
	public:
		using Event::wait;
		/** Whether the future was fulfilled. */
		inline bool fulfilled() const;

		/** Initialise the future. */
		void initialise();

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