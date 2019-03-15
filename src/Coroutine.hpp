/** @file Coroutine.hpp
	Contains the coroutine base type. */
#ifndef __libcr_coroutine_hpp_defined
#define __libcr_coroutine_hpp_defined

#include "Context.hpp"
#include "Protothread.hpp"
#include "detail/NextPointer.hpp"

#include <atomic>
#include <cstddef>
#include <utility>

namespace cr
{
	/** Base class for coroutines.
		Coroutines are optimised for deeper nesting than protothreads, and entering into a callstack has constant complexity instead of linear complexity. */
	class Coroutine : public Protothread
	{
	public:
		/** Coroutine implementation pointer type.
			This is used to call coroutines of unknown deriving type. */
		typedef void (Coroutine::*impl_t)();

		/** The coroutine's "thread_local" storage. */
		Context * libcr_context;
		/** The coroutine's parent coroutine (or null). */
		Coroutine * libcr_parent;
		/** The coroutine implementation.
			Used to enter a coroutine. */
		impl_t libcr_coroutine;
		/** Error flag that can be set when a blocking operation fails. */
		bool libcr_error;
		/** When waiting for a resource, the next coroutine in line, or null if last. */
		detail::NextPointer libcr_next_waiting;

		/** Prepares the coroutine to be the root coroutine.
		@param[in] coroutine:
			The coroutine implementation. */
		void prepare(
			impl_t coroutine,
			Context * context);

		/** Prepares the coroutine to be a child coroutine.
		@param[in] coroutine:
			The coroutine implementation.
		@param[in] parent:
			The parent coroutine. */
		void prepare(
			impl_t coroutine,
			Coroutine * parent);

		/** Directly progresses the coroutine until its next suspension.
			This should only be called by library functions. */
		inline void operator()();

		template<class T>
		/** Short-hand for `Context::local()`. */
		inline T &local();

		template<class T>
		/** Waits for an awaitable object.
		@param[in] value:
			An awaitable object.
		@return
			Whether the coroutine does not need to wait. */
		inline bool libcr_unpack_wait(
			T * value);

		template<class T>
		/** Waits for an awaitable object.
		@param[in] value:
			An awaitable object.
		@return
			Whether the coroutine does not need to wait. */
		inline bool libcr_unpack_wait(
			T & value);

		template<class T>
		/** Waits for an awaitable object.
		@param[in] value:
			An awaitable object.
		@return
			Whether the coroutine does not need to wait. */
		inline bool libcr_unpack_wait(
			T && value);

		/** Waits for an awaitable object.
		@param[in] value:
			Whether the coroutine does not need to wait.
		@return
			Whether the coroutine does not need to wait. */
		inline bool libcr_unpack_wait(
			bool value);
	};
}

#include "Coroutine.inl"

#endif