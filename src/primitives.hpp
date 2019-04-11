/** @file primitives.hpp
	Contains macros to be used as primitives for coroutines. */
#ifndef __libcr_primitives_hpp_defined
#define __libcr_primitives_hpp_defined

#include "helpermacros.hpp"
#include "specific.hpp"

#include "Protothread.hpp"
#include "Coroutine.hpp"
#include "sync/Block.hpp"

#include "detail/CoroutineHelper.hpp"
#include "detail/ExposeCoroutine.hpp"
#include "detail/ProtothreadHelper.hpp"

/** @def CR_CHECKPOINT
	Saves the execution for restoring it later. */
#define CR_CHECKPOINT LIBCR_HELPER_CHECKPOINT(__COUNTER__)
#define LIBCR_HELPER_CHECKPOINT(id) do { \
	LIBCR_HELPER_SAVE(id); \
	LIBCR_HELPER_LABEL(id):; \
} while(0)

/** @def CR_YIELD
	Yields the coroutine, and waits for the scheduler to resume it.
	Only works with nest coroutines. */
#define CR_YIELD CR_AWAIT(LibCrScheduler::instance().enqueue())

/** @def CR_PYIELD
	Saves the execution progress and yields the execution to the calling function.
	Only works with protothreads. */
#define PT_YIELD LIBCR_HELPER_PYIELD(__COUNTER__)
#define LIBCR_HELPER_PYIELD(id) do { \
	LIBCR_HELPER_ASSERT_PROTOTHREAD_SELF("PT_YIELD"); \
	LIBCR_HELPER_SAVE(id); \
	return false; \
	LIBCR_HELPER_LABEL(id):; \
} while(0)

/** @def CR_AWAIT(operation, [error])
	If `operation` is blocking, yields the coroutine until the operation finished. `error` is an optional argument, and contains code that is used to handle errors (no trailing semicolon is needed). Only works with nest coroutines.

	Example:

		CR_AWAIT(cv.wait(), {
			something();
		});

	This is equal to the following pseudocode:

		try { await cv.wait(); }
		catch(...) { something(); } */
#define CR_AWAIT(...) LIBCR_HELPER_OVERLOAD(LIBCR_HELPER_AWAIT, __VA_ARGS__)

#define LIBCR_HELPER_AWAIT1(operation) LIBCR_HELPER_AWAIT2(operation, CR_THROW)
#define LIBCR_HELPER_AWAIT2(operation, error) LIBCR_HELPER_AWAIT(__COUNTER__, operation, error)
#define LIBCR_HELPER_AWAIT(id, operation, error) do { \
	LIBCR_HELPER_ASSERT_COROUTINE_SELF("CR_AWAIT"); \
	LIBCR_HELPER_SAVE(id); \
	if(::cr::sync::block() == ::cr::Coroutine::libcr_unpack_wait(operation)) \
		return; \
	LIBCR_HELPER_LABEL(id):; \
	if(::cr::Coroutine::libcr_error) \
	{ \
		::cr::Coroutine::libcr_error = false; \
		error; \
	} \
} while(0)

/** @def CR_THROW
	Terminates the coroutine, and executes the error handler in the `#CR_CALL` that invoked this coroutine. Must not be used in coroutines that have no calling parent coroutine. Constructions for passing error values must be created manually. Only works with nest coroutines. */
#define CR_THROW do { \
	LIBCR_HELPER_ASSERT_COROUTINE_SELF("CR_THROW"); \
	assert(::cr::Coroutine::libcr_parent && "Cannot throw in coroutines that have no parent coroutine!"); \
	::cr::Coroutine::libcr_parent->libcr_error = true; \
	CR_RETURN; \
} while(0)

/** @def CR_RETURN
	Returns from a coroutine to the caller or its parent coroutine. Marks the coroutine as "done", and the coroutine must not be called again. */
#define CR_RETURN do { \
	LIBCR_HELPER_ASSERT_COROUTINE_SELF("CR_RETURN"); \
	goto cr_label_return; \
} while(0)

/** @def PT_RETURN
	Returns from a protothread to the caller or its parent coroutine. Marks the coroutine as "done", and the coroutine must not be called again. */
#ifdef LIBCR_DEBUG
	#define PT_RETURN do { \
		LIBCR_HELPER_ASSERT_PROTOTHREAD_SELF("PT_RETURN"); \
		goto cr_label_return; \
	} while(0)
#else
	#define PT_RETURN do { \
		LIBCR_HELPER_ASSERT_PROTOTHREAD_SELF("PT_RETURN"); \
		return true; \
	} while(0)
#endif

/** @def CR_CALL(coroutine, args, [error])
	Calls a coroutine from inside another coroutine. `args` has to be surrounded with parentheses. `error` is an optional argument and contains error-handling code that is executed when the called coroutine exits via `#CR_THROW` (no trailing semicolon needed). If `error` is omitted, it defaults to `#CR_THROW`. Behaves equivalent to an `await` statement, the control flow only resumes after the called coroutine is done.

	Example:

		CR_CALL(receive, (connection, buffer, buffer_size), {
			something();
		});

	This is equivalent to the following pseudocode:

		try { await connection.receive(buffer, buffer_size); }
		catch(...) { something(); } */
#define CR_CALL(...) LIBCR_HELPER_OVERLOAD(LIBCR_HELPER_CALL, __VA_ARGS__)
#define LIBCR_HELPER_CALL2(coroutine, args) LIBCR_HELPER_CALL3(coroutine, args, CR_THROW)
#define LIBCR_HELPER_CALL3(coroutine, args, error) LIBCR_HELPER_CALL(__COUNTER__, coroutine, args, error)
#define LIBCR_HELPER_CALL(id, coroutine, args, error) do { \
	LIBCR_HELPER_ASSERT_COROUTINE_SELF("CR_CALL"); \
	LIBCR_HELPER_ASSERT_COROUTINE("CR_CALL", decltype(coroutine)); \
	LIBCR_HELPER_CALL_PREFIX(id, coroutine); \
	coroutine.start LIBCR_HELPER_PREPARE args; \
	LIBCR_HELPER_CALL_SUFFIX(id, coroutine, error); \
} while(0)
#define LIBCR_HELPER_CALL_PREFIX(id, coroutine) do {\
	LIBCR_HELPER_ASSERT_COROUTINE_SELF("CR_HELPER_CALL_PREFIX"); \
	LIBCR_HELPER_ASSERT_COROUTINE("CR_HELPER_CALL_PREFIX", decltype(coroutine)); \
	LIBCR_HELPER_SAVE(id); \
} while(0)

#define LIBCR_HELPER_CALL_SUFFIX(id, coroutine, error) do {\
	LIBCR_HELPER_ASSERT_COROUTINE_SELF("CR_HELPER_CALL_SUFFIX"); \
	LIBCR_HELPER_ASSERT_COROUTINE("CR_HELPER_CALL_SUFFIX", decltype(coroutine)); \
	return; \
	LIBCR_HELPER_LABEL(id):; \
	if(::cr::Coroutine::libcr_error) \
	{ \
		::cr::Coroutine::libcr_error = false; \
		error; \
	} \
} while(0)

#define CR_CALL_PREPARED(...) LIBCR_HELPER_OVERLOAD(LIBCR_HELPER_CALL_PREPARED, __VA_ARGS__)
#define LIBCR_HELPER_CALL_PREPARED1(coroutine) LIBCR_HELPER_CALL_PREPARED2(coroutine, CR_THROW)
#define LIBCR_HELPER_CALL_PREPARED2(coroutine, error) LIBCR_HELPER_CALL_PREPARED(__COUNTER__, coroutine, error)
#define LIBCR_HELPER_CALL_PREPARED(id, coroutine, error) do { \
	LIBCR_HELPER_CALL_PREFIX(id, coroutine); \
	coroutine.start_prepared(); \
	LIBCR_HELPER_CALL_SUFFIX(id, coroutine, error); \
} while(0)


/** @def PT_CALL(state, args)
	Calls a protothread from inside another protothread. Sets a checkpoint before calling, so it behaves like a regular function call. `args` is used to initialise the coroutine, and must be wrapped in parentheses. Behaves equivalent to an `await` statement, the control flow only resumes after the called protothread is done. */
#define PT_CALL(state, args) do { \
			LIBCR_HELPER_ASSERT_PROTOTHREAD("PT_CALL", decltype(state)); \
			(state).prepare args; \
			CR_CHECKPOINT; \
			if(!(state)()) \
				return false; \
		} while(0)

/** @def PT_CALL_NAKED(state)
	Calls a protothread from inside another coroutine. Does not set a checkpoint before calling. The coroutine needs to be initialised manually beforehand.
	This behaves similar to an `await` statement, the control flow only resumes after the called coroutine is done, but it is not blocking the calling coroutine's control flow because the coroutine's previous checkpoint remains active. This means that one can use this to have multiple simultaneous coroutine calls within a single coroutine. */
#define PT_CALL_NAKED(state) do { \
			LIBCR_HELPER_ASSERT_PROTOTHREAD("PT_CALL_NAKED", decltype(state)); \
			if(!state()) \
				return false; \
		} while(0)


/** @def PT_IMPL_END
	Marks the end of a protothread implementation. */
/** @def CR_IMPL_END
	Marks the end of a coroutine implementation. */
#ifdef LIBCR_DEBUG
	#define PT_IMPL_END LIBCR_HELPER_PIMPL_END(__COUNTER__)
	#define LIBCR_HELPER_PIMPL_END(id) do { \
			LIBCR_HELPER_ASSERT_PROTOTHREAD_SELF("PT_IMPL_END"); \
			/* To suppress the "unused" warning on the label. */ \
			goto cr_label_return; \
		cr_label_return: \
			LIBCR_HELPER_SAVE(id); \
			return true; \
		LIBCR_HELPER_LABEL(id): \
			assert(!"PROTOTHREAD coroutine called after return."); \
		} while(0); \
	}

	#define CR_IMPL_END LIBCR_HELPER_IMPL_END(__COUNTER__)
	#define LIBCR_HELPER_IMPL_END(id) do { \
			LIBCR_HELPER_ASSERT_COROUTINE_SELF("CR_IMPL_END"); \
			LIBCR_HELPER_SAVE(id); \
			/* To suppress the "unused" warning on the label. */ \
			goto cr_label_return; \
		cr_label_return: \
			if(::cr::Coroutine::libcr_parent) \
			{ \
				::cr::Coroutine &parent = *::cr::Coroutine::libcr_parent; \
				parent.libcr_thread = ::cr::Coroutine::libcr_thread; \
				cr_destroy(); \
				parent(); \
				return; \
			} else \
			{ \
				cr_destroy(); \
				return; \
			} \
		LIBCR_HELPER_LABEL(id): \
			assert(!"COROUTINE coroutine called after return."); \
		} while(0); \
	}
#else
	#define PT_IMPL_END do { \
			LIBCR_HELPER_ASSERT_PROTOTHREAD_SELF("PT_IMPL_END"); \
			/* To suppress the "unused" warning on the label. */ \
			goto cr_label_return; \
		cr_label_return: \
			return true; \
		} while(0); \
	}
	#define CR_IMPL_END do { \
			LIBCR_HELPER_ASSERT_COROUTINE_SELF("CR_IMPL_END"); \
			/* To suppress the "unused" warning on the label. */ \
			goto cr_label_return; \
		cr_label_return: \
			if(::cr::Coroutine::libcr_parent) \
			{ \
				::cr::Coroutine &parent = *::cr::Coroutine::libcr_parent; \
				parent.libcr_thread = ::cr::Coroutine::libcr_thread; \
				cr_destroy(); \
				parent(); \
				return; \
			} else \
			{ \
				cr_destroy(); \
				return; \
			} \
		} while(0); \
	}
#endif

#define LIBCR_HELPER_INTRO do { \
	if(::cr::Protothread::libcr_instruction_pointer) \
		CR_RESTORE; \
} while(0)

/** @def CR_IMPL(name)
	Starts the external implementation of the given coroutine. Must be followed by `#CR_IMPL_END`. */
#define CR_IMPL(...) void __VA_ARGS__::_cr_implementation() \
{ \
	LIBCR_HELPER_ASSERT_COROUTINE_SELF("CR_IMPL"); \
	LIBCR_HELPER_INTRO;


/** @def PT_IMPL(name)
	Starts the external implementation of the given protothread. Must be followed by `#PT_IMPL_END`. */
#define PT_IMPL(...) bool __VA_ARGS__::_cr_implementation() \
{ \
	LIBCR_HELPER_ASSERT_PROTOTHREAD_SELF("PT_IMPL"); \
	LIBCR_HELPER_INTRO;

/** @def PT_INLINE
	Starts the inline implementation of the given protothread. Must be followed by `#PT_INLINE_END`. */
#define PT_INLINE private:bool _cr_implementation() \
{ \
	LIBCR_HELPER_ASSERT_PROTOTHREAD_SELF("PT_INLINE"); \
	LIBCR_HELPER_INTRO;

/** @def PT_INLINE_END
	Ends the inline implementation and definition of a protothread declaration. */
#define PT_INLINE_END \
	LIBCR_HELPER_ASSERT_PROTOTHREAD_SELF("PT_INLINE_END"); \
	PT_IMPL_END \
};

/** @def CR_INLINE
	Starts the inline implementation of the given coroutine. Must be followed by `#CR_INLINE_END`. */
#define CR_INLINE private:void _cr_implementation() \
{ \
	LIBCR_HELPER_ASSERT_COROUTINE_SELF("CR_INLINE"); \
	LIBCR_HELPER_INTRO;

/** @def CR_INLINE_END
	Ends the inline implementation and definition of a coroutine declaration. */
#define CR_INLINE_END \
	LIBCR_HELPER_ASSERT_COROUTINE_SELF("CR_INLINE_END"); \
	CR_IMPL_END \
};


/** @def CR_EXTERNAL
	Ends the definition of a coroutine declaration, and marks it as externally implemented. Use `#CR_IMPL` to implement the coroutine. */
#define CR_EXTERNAL private:void _cr_implementation(); \
};
/** @def CR_EXTERNAL_INLINE
	Ends the definition of a coroutine declaration, and marks it as externally implemented, using the `inline` specifier. Use `#CR_IMPL` to implement the coroutine. */
#define CR_EXTERNAL_INLINE private:inline void _cr_implementation(); \
};

/** @def PT_EXTERNAL
	Ends the definition of a protothread declaration, and marks it as externally implemented. Use `#PT_IMPL` to implement the coroutine. */
#define PT_EXTERNAL private:bool _cr_implementation(); \
};
/** @def PT_EXTERNAL_INLINE
	Ends the definition of a protothread declaration, and marks it as externally implemented, using the `inline` specifier. Use `#PT_IMPL` to implement the coroutine. */
#define PT_EXTERNAL_INLINE private:inline bool _cr_implementation(); \
};

/** @def COROUTINE(name, scheduler, inheritance)
	Creates a coroutine with the given name.
	Nested coroutines are optimised for deeper nesting, and entering into a callstack has constant complexity instead of linear complexity.
	`inheritance` is the inheritance list of the coroutine.

	For coroutines with templates, use `#TEMPLATE_COROUTINE`. */
#define COROUTINE(name, scheduler, ...) LIBCR_HELPER_COROUTINE(name, (), scheduler, private ::cr::detail::CoroutineHelper<name>, ::cr::detail::ExposeCoroutine, ##__VA_ARGS__)
/** @def TEMPLATE_COROUTINE(name, templates, inheritance)
	Creates a templated coroutine class.
	`templates` must be the coroutine template arguments, enclosed in parentheses.
	`scheduler` must be the coroutine's scheduler type.
	Example:

		template<class T, std::size_t size>
		TEMPLATE_COROUTINE(Array, (T, size), cr::mt::Scheduler) */
#define TEMPLATE_COROUTINE(name, templates, scheduler, ...) LIBCR_HELPER_COROUTINE(name, (<LIBCR_HELPER_UNPACK templates>), scheduler, private ::cr::detail::CoroutineHelper<name<LIBCR_HELPER_UNPACK templates>>, ::cr::detail::ExposeCoroutine, ##__VA_ARGS__)
#define LIBCR_HELPER_COROUTINE(name, templates, scheduler, ...) \
class name : __VA_ARGS__ \
{ \
	friend class ::cr::detail::CoroutineHelper<name LIBCR_HELPER_UNPACK templates>; \
	typedef name LIBCR_HELPER_UNPACK templates LibCrSelf; \
	typedef ::cr::detail::CoroutineHelper<name LIBCR_HELPER_UNPACK templates> LibCrBase; \
	friend class ::cr::detail::ExposeCoroutine; \
	typedef scheduler LibCrScheduler; \
public: \
	using LibCrBase::start; \
	using LibCrBase::prepare; \
	using LibCrBase::start_prepared; \
	name() = default; \
	template<class Arg0, class ...Args> \
	name(Arg0 arg0, Args&& ...args) \
	{ \
		start( \
			arg0, \
			std::forward<Args>(args)...); \
	}

/** @def PROTOTHREAD(name, inheritance)
	Creates a protothread with the given name.
	Plain coroutines are not optimised for nesting, entering into a callstack has linear complexity. However, protothreads have less restrictions on their behaviour, and can be used to have multiple "simultaneous" child coroutines, using `#CR_PCALL_NAKED`.
	`inheritance` is the inheritance list of the coroutine.

	For coroutines with templates, use `#TEMPLATE_PROTOTHREAD`. */
#define PROTOTHREAD(name, ...) LIBCR_HELPER_PROTOTHREAD(name, (), private ::cr::detail::ProtothreadHelper<name>, ##__VA_ARGS__)
/** @def TEMPLATE_PROTOTHREAD(name, templates, inheritance)
	Creates a templated protothread class.
	`templates` must be the coroutine template arguments, enclosed in parentheses.
	Example:

		template<class T, std::size_t size>
		TEMPLATE_PROTOTHREAD(Array, (T, size)) */
#define TEMPLATE_PROTOTHREAD(name, templates, ...) LIBCR_HELPER_PROTOTHREAD(name, (<LIBCR_HELPER_UNPACK templates>), private ::cr::detail::ProtothreadHelper<name<LIBCR_HELPER_UNPACK templates>>, ##__VA_ARGS__)
#define LIBCR_HELPER_PROTOTHREAD(name, templates, ...) \
class name : __VA_ARGS__ \
{ \
	friend class ::cr::detail::ProtothreadHelper<name LIBCR_HELPER_UNPACK templates>; \
	typedef name LIBCR_HELPER_UNPACK templates LibCrSelf; \
	typedef ::cr::detail::ProtothreadHelper<name LIBCR_HELPER_UNPACK templates> LibCrBase; \
public: \
	using LibCrBase::operator(); \
	using LibCrBase::prepare;


/** @def CR_STATE(args)
	Starts the internal / state variable section of a coroutine. Automatically generates the arguments and a cr_prepare function that takes the arguments. The arguments have to be of the form `(type) name`. If no automatically generated prepare function is desired, use `#CR_STATE_NOAUTO`. */
#define CR_STATE(...) \
private: \
	LIBCR_HELPER_CREATE_PREPARE(__VA_ARGS__)

/** @def CR_STATE_NOAUTO
	Starts the internal / state variable section of a coroutine. */
#define CR_STATE_NOAUTO \
private:

/** @def PT_STATE(args)
	Starts the internal / state variable section of a protothread. Automatically generates the arguments and a cr_prepare function that takes the arguments. The arguments have to be of the form `(type) name`. If no automatically generated prepare function is desired, use `#PT_STATE_NOAUTO`. */
#define PT_STATE(...) \
private: \
	LIBCR_HELPER_CREATE_PREPARE(__VA_ARGS__)

/** @def CR_STATE_NOAUTO
	Starts the internal / state variable section of a protothread. */
#define PT_STATE_NOAUTO \
private:

#endif