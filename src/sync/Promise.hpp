/** @file Promise.hpp
	Contains the promise type. */
#ifndef __libcr_src_sync_promise_hpp
#define __libcr_src_sync_promise_hpp

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
	class PODPromiseBaseBase
	{
	protected:
		/** The promise's future. */
		PODFutureBase<T, Event> * m_future;
	public:
		/** Initialises the promise without a future. */
		void initialise();
		/** Initialises the promise to be bound to the given future.
		@param[in] future:
			The future to use with this promise. */
		void initialise(
			PODFutureBase<T, Event> * future);
		/** Initialises the promise to be bound to the given future.
		@param[in] future:
			The future to use with this promise. */
		inline void initialise(
			FutureBase<T, Event> * future);
	};

	template<class T, class Event>
	/** POD promise type.
	@tparam T:
		The promise's value type.
	@tparam Event:
		The promise's future's event flavour. */
	class PODPromiseBase;

	template<class Event>
	/** POD value-less promise type.
	@tparam Event:
		The promise's future's even flavour. */
	class PODPromiseBase<void, Event> : PODPromiseBaseBase<void, Event>
	{
		using PODPromiseBaseBase<void, Event>::m_future;
	public:
		using PODPromiseBaseBase<void, Event>::initialise;

		/** Fulfills the promise.
			The promise must be bound to a future. */
		void fulfill();
	};

	template<class T, class Event>
	class PODPromiseBase : PODPromiseBaseBase<T, Event>
	{
		using PODPromiseBaseBase<T, Event>::m_future;
	public:
		using PODPromiseBaseBase<T, Event>::initialise;

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
	class PromiseBase : PODPromiseBase<T, Event>
	{
	public:
		using PODPromiseBase<T, Event>::fulfill;
		/** Initialises the promise without a future. */
		PromiseBase();
		/** Initialises the promise to be bound to the given future.
		@param[in] future:
			The future to use with this promise. */
		PromiseBase(
			PODFutureBase<T, Event> * future);
		/** Initialises the promise to be bound to the given future.
		@param[in] future:
			The future to use with this promise. */
		PromiseBase(
			FutureBase<T, Event> * future);
	};

	template<class T>
	/** POD promise type.
	@tparam T:
		The promise's value type. */
	using PODPromise = PODPromiseBase<T, PODEvent>;
	template<class T>
	/** POD promise type supporting only one waiting coroutine.
	@tparam T:
		The promise's value type. */
	using PODFIFOPromise = PODPromiseBase<T, PODFIFOEvent>;
	template<class T>
	/** Promise type.
	@tparam T:
		The promise's value type. */
	using Promise = PromiseBase<T, PODEvent>;
	template<class T>
	/** Promise type supporting only one waiting coroutine.
	@tparam T:
		The promise's value type. */
	using FIFOPromise = PromiseBase<T, PODFIFOEvent>;
}

#include "Promise.inl"

#endif