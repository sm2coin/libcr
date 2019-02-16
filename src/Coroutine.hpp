/** @file Coroutine.hpp
	The coroutine header. */
#ifndef __libcr_coroutine_hpp_defined
#define __libcr_coroutine_hpp_defined

#include "Context.hpp"
#include "primitives.hpp"

#include <atomic>
#include <cstddef>
#include <utility>

#ifdef LIBCR_DEBUG
/** @def LIBCR_MAGIC_NUMBER
	A random number that is used to check whether a coroutine was properly initialised. */
#define LIBCR_MAGIC_NUMBER 0x38eee375b1b29314
#endif

namespace cr
{
	/** Instruction pointer type.
		This is used to save the execution of a coroutine. */
	typedef void *ip_t;

	/** Basic coroutine state. */
	class PlainCoroutine
	{
	public:
#ifdef LIBCR_DEBUG
		/** Used when debugging, to check whether the coroutine was initialised. */
		std::size_t libcr_magic_number;
#endif
		/** Contains the last saved instruction pointer. */
		ip_t libcr_coroutine_ip;

		/** Prepares the coroutine for execution. */
		void prepare();
	};

	template<class DerivedCoroutine>
	/** Helper class for easier initialisation of plain coroutines.
		Plain coroutines are not optimised for nesting, entering into a callstack has linear complexity. However, plain coroutines have less restrictions on their behaviour, and can be used to have multiple "simultaneous" child coroutines, using `#CR_PCALL_NAKED`.
	@tparam DerivedCoroutine:
		The coroutine type that derives from this type. */
	class PlainCoroutineHelper : protected PlainCoroutine
	{
	public:
		/** Executes the coroutine.
		@return
			Whether the coroutine is done. */
		inline bool operator()();
	};

	template<class T>
	class CoroutineHelper;

	/** Base class for nested coroutines.
		Nested coroutines are optimised for deeper nesting, and entering into a callstack has constant complexity instead of linear complexity. */
	class Coroutine : public PlainCoroutine
	{
	public:
		/** Coroutine implementation pointer type.
			This is used to call coroutines of unknown deriving type.
		@param[in] self:
			The coroutine state to execute.
		@return
			Whether the coroutine is done. */
		typedef void (Coroutine::*impl_t)();

		/** The coroutine's "thread_local" storage. */
		Context * libcr_context;
		/** The coroutine's parent coroutine (or null). */
		Coroutine * libcr_parent;
		/** The coroutine implementation.
			Used to enter a coroutine. */
		impl_t libcr_coroutine;

		/** When waiting for a resource, the next coroutine in line, or self if last. */
		std::atomic<Coroutine *> libcr_next_waiting;

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


		/** Whether a coroutine is waiting. */
		inline bool waiting() const;

		/** The next coroutine waiting, or null. */
		inline Coroutine * next_waiting() const;

		/** Sets a coroutine to be waiting.
			The coroutine must not be waiting already. */
		void pause();

		/** Marks a waiting coroutine as no longer waiting.
			The coroutine must be waiting. Does not execute the coroutine. */
		void resume();
	};

	/** Helper class that exposes the nested coroutine base to other nested coroutines. */
	class ExposeCoroutine
	{
		template<class T>
		friend class CoroutineHelper;
	protected:
		template<
			class T,
			class = typename std::enable_if<
				std::is_base_of<Coroutine, T>::value
			>::type>
		/** Returns the nested coroutine base pointer of a nested coroutine.
		@param[in] coroutine:
			The coroutine whose base to expose.
		@return
			The coroutine's base. */
		static constexpr Coroutine * base(
			T * coroutine);

		template<
			class T,
			class = typename std::enable_if<
				std::is_base_of<Coroutine, T>::value
			>::type>
		/** Directly calls a coroutine (link time address). */
		static inline void invoke(
			T &coroutine);
	};

	template<class DerivedCoroutine>
	/** Helper type for initialising and calling nested coroutines.
	@tparam DerivedCoroutine:
		The deriving type. */
	class CoroutineHelper : public Coroutine
	{
		friend DerivedCoroutine;
	public:
		template<class ...Args>
		/** Prepares the coroutine as the root coroutine. */
		inline void prepare(
			Context * context,
			Args&& ...args);

		template<class ...Args>
		/** Prepares the coroutine as a child coroutine.
		@param[in] parent:
			The parent coroutine. */
		inline void prepare(
			Coroutine * parent,
			Args&& ...args);

		template<class ...Args>
		/** Prepares the coroutine as the root coroutine, without a context. */
		inline void prepare(
			nullptr_t,
			Args&& ...args);

		/** Starts the coroutine.
			This should only be called once per coroutine! */
		inline void start();
	};
}

#include "Coroutine.inl"

#endif