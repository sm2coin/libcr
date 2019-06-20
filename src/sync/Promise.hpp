/** @file Promise.hpp
	Contains the promise type. */
#ifndef __libcr_sync_promise_hpp_defined
#define __libcr_sync_promise_hpp_defined

#include "Future.hpp"
#include "../util/Argument.hpp"

namespace cr::sync
{
	template<class T, class Event>
	/** POD promise base type.
	@tparam T:
		The promise's value type.
	@tparam Event:
		The promise's future's event flavour. */
	class PODPromiseBasePattern
	{
	protected:
		/** The promise's future. */
		PODFuturePattern<T, Event> * m_future;
	public:
		/** Initialises the promise without a future. */
		void initialise();
		/** Initialises the promise to be bound to the given future.
		@param[in] future:
			The future to use with this promise. */
		void initialise(
			PODFuturePattern<T, Event> * future);
		/** Initialises the promise to be bound to the given future.
		@param[in] future:
			The future to use with this promise. */
		inline void initialise(
			FuturePattern<T, Event> * future);
	};

	template<class T, class Event>
	/** POD promise type.
	@tparam T:
		The promise's value type.
	@tparam Event:
		The promise's future's event flavour. */
	class PODPromisePattern;

	template<class Event>
	/** POD value-less promise type.
	@tparam Event:
		The promise's future's even flavour. */
	class PODPromisePattern<void, Event> : PODPromiseBasePattern<void, Event>
	{
		using PODPromiseBasePattern<void, Event>::m_future;
	public:
		using PODPromiseBasePattern<void, Event>::initialise;

		/** Fulfills the promise.
			The promise must be bound to a future. */
		void fulfill();
	};

	template<class T, class Event>
	class PODPromisePattern : PODPromiseBasePattern<T, Event>
	{
		using PODPromiseBasePattern<T, Event>::m_future;
	public:
		using PODPromiseBasePattern<T, Event>::initialise;

		/** Fulfills the promise.
			The promise must be bound to a future.
		@param[in] value:
			The value to fulfill the promise with. */
		void fulfill(
			T const& value);
		/** Fulfills the promise.
			The promise must be bound to a future.
		@param[in] value:
			The value to fulfill the promise with. */
		void fulfill(
			T && value);
	};

	template<class T, class Event>
	/** Promise type.
	@tparam T:
		The promise's value type.
	@tparam Event:
		The promise's future's event flavour. */
	class PromisePattern : PODPromisePattern<T, Event>
	{
	public:
		using PODPromisePattern<T, Event>::fulfill;
		/** Initialises the promise without a future. */
		PromisePattern();
		/** Initialises the promise to be bound to the given future.
		@param[in] future:
			The future to use with this promise. */
		PromisePattern(
			PODFuturePattern<T, Event> * future);
		/** Initialises the promise to be bound to the given future.
		@param[in] future:
			The future to use with this promise. */
		PromisePattern(
			FuturePattern<T, Event> * future);
	};

	template<class T>
	/** POD promise type.
	@tparam T:
		The promise's value type. */
	using PODPromise = PODPromisePattern<T, PODEvent>;
	template<class T>
	/** POD promise type supporting only one waiting coroutine.
	@tparam T:
		The promise's value type. */
	using PODFIFOPromise = PODPromisePattern<T, PODFIFOEvent>;
	template<class T>
	/** Promise type.
	@tparam T:
		The promise's value type. */
	using Promise = PromisePattern<T, PODEvent>;
	template<class T>
	/** Promise type supporting only one waiting coroutine.
	@tparam T:
		The promise's value type. */
	using FIFOPromise = PromisePattern<T, PODFIFOEvent>;
}

#include "Promise.inl"

#endif