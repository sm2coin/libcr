#ifndef __coroutines_coroutines_hpp_defined
#define __coroutines_coroutines_hpp_defined

#include "primitives.hpp"

#include <cstddef>
#include <utility>

/** @def LIBCR_MAGIC_NUMBER
	A random number that is used to check whether a coroutine was properly initialised. */
#define LIBCR_MAGIC_NUMBER 0x38eee375b1b29314

namespace cr
{
	/** Instruction pointer type.
		This is used to save the execution of a coroutine. */
	typedef void *ip_t;
	/** Coroutine implementation pointer type.
		This is used to call coroutines of unknown type.
	@param[in] self:
		The coroutine state to execute.
	@return
		Whether the coroutine is done. */
	typedef bool (*impl_t)(void * self);

	/** Basic coroutine state. */
	class CoroutineBase
	{
	public:
#ifdef LIBCR_DEBUG
		/** Used when debugging, to check whether the coroutine was initialised. */
		std::size_t m_magic_number;
#endif
		/** The coroutine implementation.
			Used to enter a coroutine. */
		impl_t m_coroutine_start;
		/** Contains the last saved instruction pointer. */
		ip_t m_coroutine_ip;

		/** Prepares the coroutine for execution.
		@param[in] coroutine_start:
			The coroutine start instruction pointer. */
		void prepare(
			impl_t coroutine_start);

		/** Calls the coroutine.
		@return
			Whether the coroutine is done. */
		inline bool operator()();
	};

	template<class DerivedCoroutine>
	/** Helper class for easier initialisation of plain coroutines.
		Plain coroutines are not optimised for nesting, entering into a callstack has linear complexity. However, plain coroutines have less restrictions on their behaviour, and can be used to have multiple "simultaneous" child coroutines, using `CR_PCALL_NAKED`.
	@tparam DerivedCoroutine:
		The coroutine type that derives from this type. */
	class Coroutine : protected CoroutineBase
	{
	protected:
		/** Prepares the coroutine. */
		inline void prepare();

	public:
		/** Executes the coroutine.
		@return
			Whether the coroutine is done. */
		inline bool operator()();
	};

	template<class T>
	class NestCoroutine;

	/** Base class for nested coroutines.
		Nested coroutines are optimised for deeper nesting, and entering into a callstack has constant complexity instead of linear complexity. */
	class NestCoroutineBase : public CoroutineBase
	{
	public:
		/** The outermost coroutine. */
		NestCoroutineBase * libcr_root;
		/** When `is_root()`, points to the deepest nested coroutine, otherwise to the immediate parent coroutine. */
		NestCoroutineBase * libcr_stack;


		/** Whether this coroutine is the outermost coroutine. */
		inline bool is_root() const;
	};

	/** Helper class that exposes the nested coroutine base to other nested coroutines. */
	class ExposeNestCoroutineBase
	{
		template<class T>
		friend class NestCoroutine;
	protected:
		template<
			class T,
			class = typename std::enable_if<
				std::is_base_of<NestCoroutineBase, T>::value
			>::type>
		/** Returns the nested coroutine base pointer of a nested coroutine.
		@param[in] coroutine:
			The coroutine whose base to expose.
		@return
			The coroutine's base. */
		static constexpr NestCoroutineBase * base(
			T * coroutine);
	};

	template<class DerivedCoroutine>
	/** Helper type for initialising and calling nested coroutines.
	@tparam DerivedCoroutine:
		The deriving type. */
	class NestCoroutine : public NestCoroutineBase
	{
		friend DerivedCoroutine;
	public:
		/** Prepares the coroutine to be the root coroutine. */
		inline void prepare();

		/** Prepares the coroutine to be a child coroutine.
		@param[in] root:
			The root coroutine.
		@param[in] parent:
			The parent coroutine. */
		inline void prepare(
			NestCoroutineBase * root,
			NestCoroutineBase * parent);

		/** Executes the coroutine.
		@return
			Whether the coroutine is donel. */
		inline bool operator()();

		/** Calls the coroutine of a `DerivedCoroutine`.
		@param[in] self:
			The coroutine state to execute.
		@return
			Whether the coroutine is done. */
		static bool lambda(
			void * self);
	};
}

#include "libcr.inl"

#endif