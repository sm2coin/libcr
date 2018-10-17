/** @file Coroutine.hpp
	The coroutine header. */
#ifndef __libcr_coroutine_hpp_defined
#define __libcr_coroutine_hpp_defined

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
		typedef bool (Coroutine::*impl_t)();

		/** The outermost coroutine. */
		Coroutine * libcr_root;
		/** When `is_root()`, points to the deepest nested coroutine, otherwise to the immediate parent coroutine. */
		Coroutine * libcr_stack;
		/** The coroutine implementation.
			Used to enter a coroutine. */
		impl_t libcr_coroutine;

		/** When waiting for a resource, the next coroutine in line, or self if last. */
		std::atomic<Coroutine *> libcr_next_waiting;

		/** Prepares the coroutine to be the root coroutine.
		@param[in] coroutine:
			The coroutine implementation. */
		void prepare(
			impl_t coroutine);

		/** Prepares the coroutine to be a child coroutine.
		@param[in] coroutine:
			The coroutine implementation.
		@param[in] parent:
			The parent coroutine. */
		void prepare(
			impl_t coroutine,
			Coroutine * parent);


		/** Whether this coroutine is the outermost coroutine. */
		inline bool is_root() const;
		/** Whether this coroutine is the innermost coroutine. */
		inline bool is_child() const;

		/** Calls the coroutine.
		@return
			Whether the coroutine is done. */
		inline bool operator()();

		/** Directly calls the coroutine, but the coroutine must be the child coroutine.
		@return
			Whether the coroutine is done. */
		inline bool directly_call_child();

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
		/** Directly calls the child coroutine.
		@return
			Whether the coroutine is done. */
		static inline bool directly_call_child(
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
		/** Prepares the coroutine as the root coroutine. */
		inline void prepare();
		/** Prepares the coroutine as a child coroutine.
		@param[in] parent:
			The parent coroutine. */
		inline void prepare(
			Coroutine * parent);
	};
}

#include "Coroutine.inl"

#endif