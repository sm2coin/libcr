#ifndef __libcr_mt_future_hpp_defined
#define __libcr_mt_future_hpp_defined

#include "Event.hpp"
#include "../util/Argument.hpp"

namespace cr::mt
{
	template<class T, class Event>
	class PODPromiseBase;
	template<class Event>
	class PODFutureBaseBase;
	template<class T, class Event>
	class PODPromiseBaseBase;

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
	class PODFutureBase<void, Event> : Event
	{
		friend class PODPromiseBase<void, Event>;
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
	class PODFutureBase : Event
	{
		friend class PODPromiseBase<T, Event>;
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
	using PODFIFOFuture = PODFutureBase<T, PODFIFOEvent>;
	template<class T>
	/** Future type that supports only one waiting coroutine. */
	using FIFOFuture = FutureBase<T, PODFIFOEvent>;
}

#include "Future.inl"

#endif