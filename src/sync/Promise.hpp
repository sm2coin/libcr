#ifndef __libcr_src_sync_promise_hpp
#define __libcr_src_sync_promise_hpp

#include "ConditionVariable.hpp"

namespace cr::sync
{
	/** Promise that can only be waited on by a single coroutine. */
	class PODSinglePromise : PODSingleConditionVariable
	{
		/** Whether the promise is fulfilled. */
		bool m_fulfilled;
	public:
		/** Initialises the promise to an unfulfilled state. */
		void initialise();

		/** Waits until the promise is fulfilled.
			If it is not yet fulfilled, blocks the coroutine until the promise is fulfilled. To be used with `#CR_AWAIT`.
		@param[in] self:
			The coroutine that waits for the promise.
		@return
			Whether the promise was already fulfilled. */
		bool wait(
			Coroutine * self);

		/** Fulfills the promise.
			Notifies all waiting coroutines. The promise must not yet be fulfilled. */
		void fulfill();
	};

	class SinglePromise : PODSinglePromise
	{
	public:
		SinglePromise();

		using PODSinglePromise::wait;
		using PODSinglePromise::fulfill;
	};

	/** Promise that can be waited on by an arbitrary amount of coroutines. */
	class PODPromise : PODConditionVariable
	{
		/** Whether the promise is fulfilled. */
		bool m_fulfilled;
	public:
		/** Initialises the promise to an unfulfilled state. */
		void initialise();

		/** Waits until the promise is fulfilled.
			If it is not yet fulfilled, blocks the coroutine until the promise is fulfilled. To be used with `#CR_AWAIT`.
		@param[in] self:
			The coroutine that waits for the promise.
		@return
			Whether the promise was already fulfilled. */
		bool wait(
			Coroutine * self);

		/** Fulfills the promise.
			Notifies all waiting coroutines. The promise must not yet be fulfilled. */
		void fulfill();
	};

	class Promise : PODPromise
	{
	public:
		Promise();

		using PODPromise::wait;
		using PODPromise::fulfill;
	};
}

#endif