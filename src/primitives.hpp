/** @file primitives.hpp
	Contains macros to be used as primitives for coroutines. */
#ifndef __libcr_primitives_hpp_defined
#define __libcr_primitives_hpp_defined

#include "helpermacros.hpp"
#include "specific.hpp"

#include "sync/Block.hpp"

/** @def CR_CHECKPOINT
	Saves the execution for restoring it later. */
#define CR_CHECKPOINT LIBCR_HELPER_CHECKPOINT(__COUNTER__)
#define LIBCR_HELPER_CHECKPOINT(id) do { \
	LIBCR_HELPER_SAVE(id); \
	LIBCR_HELPER_LABEL(id):; \
} while(0)

/** @def CR_YIELD
	Saves the execution progress and yields the execution to the calling function. */
#define CR_YIELD LIBCR_HELPER_YIELD(__COUNTER__)
#define LIBCR_HELPER_YIELD(id) do { \
	LIBCR_HELPER_SAVE(id); \
	return false; \
	LIBCR_HELPER_LABEL(id):; \
} while(0)

/** @def CR_AWAIT(operation)
	If `operation` is blocking, yields the coroutine until the operation finished.
	Only works with nest coroutines. */
#define CR_AWAIT(operation) LIBCR_HELPER_AWAIT(__COUNTER__, operation)
#define LIBCR_HELPER_AWAIT(id, operation) do { \
	LIBCR_HELPER_ASSERT_NESTED_SELF("CR_AWAIT"); \
	LIBCR_HELPER_SAVE(id); \
	if(::cr::sync::block() == ::cr::Coroutine::libcr_unpack_wait(operation)) \
	{ \
		do { \
			return false; \
			LIBCR_HELPER_LABEL(id):; \
		} while(waiting()); \
	} \
} while(0)

/** @def CR_RETURN
	Returns from a nested coroutine to the caller or its parent coroutine. Marks the coroutine as "done", and the coroutine must not be called again. */
#define CR_RETURN do { \
	LIBCR_HELPER_ASSERT_NESTED_SELF("CR_RETURN"); \
	goto cr_label_return; \
} while(0)

/** @def CR_PRETURN
	Returns from a plain coroutine to the caller or its parent coroutine. Marks the coroutine as "done", and the coroutine must not be called again. */
#ifdef LIBCR_DEBUG
	#define CR_PRETURN do { \
		LIBCR_HELPER_ASSERT_PLAIN_SELF("CR_PRETURN"); \
		goto cr_label_return; \
	} while(0)
#else
	#define CR_PRETURN do { \
		LIBCR_HELPER_ASSERT_PLAIN_SELF("CR_PRETURN"); \
		return true; \
	} while(0)
#endif

/** @def CR_CALL(state, args)
	Calls a nested coroutine from inside another nested coroutine.
	Behaves equivalent to an `await` statement, the control flow only resumes after the called coroutine is done. */
#define CR_CALL(state, ...) LIBCR_HELPER_CALL(__COUNTER__, state, ##__VA_ARGS__)
#define LIBCR_HELPER_CALL(id, ...) do { \
	LIBCR_HELPER_ASSERT_CHILD; \
	LIBCR_HELPER_ASSERT_NESTED("CR_CALL", decltype(LIBCR_HELPER_HEAD(__VA_ARGS__))); \
	LIBCR_HELPER_PREPARE(__VA_ARGS__, this); \
	::cr::Coroutine::libcr_root->libcr_stack = ::cr::ExposeCoroutine::base(&(LIBCR_HELPER_HEAD(__VA_ARGS__))); \
	LIBCR_HELPER_SAVE(id); \
	return ::cr::ExposeCoroutine::directly_call_child(LIBCR_HELPER_HEAD(__VA_ARGS__)); \
	LIBCR_HELPER_LABEL(id):; \
} while(0)

/** @def CR_PCALL(state, args)
	Calls a plain coroutine from inside another coroutine. Sets a checkpoint before calling, so it behaves like a regular function call. `args` is used to initialise the coroutine.
	Behaves equivalent to an `await` statement, the control flow only resumes after the called coroutine is done. */
#define CR_PCALL(state,...) do { \
			LIBCR_HELPER_ASSERT_PLAIN("CR_CALL_PLAIN", decltype(state)); \
			(state).prepare(__VA_ARGS__); \
			CR_CHECKPOINT; \
			if(!(state)()) \
				return false; \
		} while(0)

/** @def CR_PCALL_NAKED(state, args)
	Calls a plain coroutine from inside another coroutine. Does not set a checkpoint before calling. The coroutine needs to be initialised manually.
	This behaves similar to an `await` statement, the control flow only resumes after the called coroutine is done, but it is not blocking the calling coroutine's control flow because the coroutine's previous checkpoint remains active. This means that one can use this to have multiple simultaneous coroutine calls within a single coroutine. */
#define CR_PCALL_NAKED(state) do { \
			LIBCR_HELPER_ASSERT_PLAIN("CR_CALL_PLAIN_NAKED", decltype(state)); \
			if(!state()) \
				return false; \
		} while(0)


/** @def CR_PIMPL_END
	Marks the end of a plain coroutine implementation. */
/** @def CR_IMPL_END
	Marks the end of a nested coroutine implementation. */
#ifdef LIBCR_DEBUG
	#define CR_PIMPL_END LIBCR_HELPER_PIMPL_END(__COUNTER__)
	#define LIBCR_HELPER_PIMPL_END(id) do { \
			LIBCR_HELPER_ASSERT_PLAIN_SELF("CR_PIMPL_END"); \
		cr_label_return: \
			LIBCR_HELPER_SAVE(id); \
			return true; \
		LIBCR_HELPER_LABEL(id): \
			assert(!"PLAIN coroutine called after return."); \
		} while(0); \
	}

	#define CR_IMPL_END LIBCR_HELPER_IMPL_END(__COUNTER__)
	#define LIBCR_HELPER_IMPL_END(id) do { \
			LIBCR_HELPER_ASSERT_NESTED_SELF("CR_IMPL_END"); \
			LIBCR_HELPER_SAVE(id); \
		cr_label_return: \
			if(::cr::Coroutine::is_root()) \
				return true; \
			else \
			{ \
				::cr::Coroutine::libcr_root->libcr_stack = ::cr::Coroutine::libcr_stack; \
				return ::cr::Coroutine::libcr_stack->directly_call_child(); \
			} \
		LIBCR_HELPER_LABEL(id): \
			assert(!"NESTED coroutine called after return."); \
		} while(0); \
	}
#else
	#define CR_PIMPL_END do { \
		cr_label_return:
			return true;
		} while(0);
	}
	#define CR_IMPL_END do { \
			LIBCR_HELPER_ASSERT_NESTED_SELF("CR_IMPL_END"); \
		cr_label_return: \
			if(is_root()) \
				return true; \
			else \
			{ \
				::cr::Coroutine::libcr_root->libcr_stack = ::cr::Coroutine::libcr_stack; \
				return ::cr::Coroutine::libcr_stack->directly_call_child(); \
			}
		} while(0); \
	}
#endif

#define LIBCR_HELPER_INTRO do { \
	if(::cr::PlainCoroutine::libcr_coroutine_ip) \
		CR_RESTORE; \
	CR_CHECKPOINT; \
} while(0)

/** @def CR_IMPL(name)
	Starts the external implementation of the given nested coroutine. Must be followed by `#CR_IMPL_END`. */
#define CR_IMPL(...) bool __VA_ARGS__::_cr_implementation() \
{ \
	LIBCR_HELPER_ASSERT_NESTED_SELF("CR_IMPL"); \
	LIBCR_HELPER_INTRO;


/** @def CR_PIMPL(name)
	Starts the external implementation of the given plain coroutine. Must be followed by `#CR_PIMPL_END`. */
#define CR_PIMPL(...) bool __VA_ARGS__::_cr_implementation() \
{ \
	LIBCR_HELPER_ASSERT_PLAIN_SELF("CR_PIMPL"); \
	LIBCR_HELPER_INTRO;

/** @def CR_PINLINE
	Starts the inline implementation of the given plain coroutine. Must be followed by `#CR_PINLINE_END`. */
#define CR_PINLINE private:bool _cr_implementation() \
{ \
	LIBCR_HELPER_ASSERT_PLAIN_SELF("CR_PIMPL"); \
	LIBCR_HELPER_INTRO;

/** @def CR_PINLINE_END
	Ends the inline implementation and definition of a plain coroutine declaration. */
#define CR_PINLINE_END \
	LIBCR_HELPER_ASSERT_PLAIN_SELF("CR_PINLINE_END"); \
	CR_PIMPL_END \
};

/** @def CR_INLINE
	Starts the inline implementation of the given nested coroutine. Must be followed by `#CR_INLINE_END`. */
#define CR_INLINE private:bool _cr_implementation() \
{ \
	LIBCR_HELPER_ASSERT_NESTED_SELF("CR_INLINE"); \
	LIBCR_HELPER_INTRO;

/** @def CR_INLINE_END
	Ends the inline implementation and definition of a nested coroutine declaration. */
#define CR_INLINE_END \
	LIBCR_HELPER_ASSERT_NESTED_SELF("CR_INLINE_END"); \
	CR_IMPL_END \
};


/** @def CR_EXTERNAL
	Ends the definition of a nested of a coroutine declaration, and marks it as externally implemented. Use `#CR_IMPL` and `#CR_PIMPL` to implement the coroutine. */
#define CR_EXTERNAL bool _cr_implementation(); \
};
/** @def CR_EXTERNAL_INLINE
	Ends the definition of a nested of a coroutine declaration, and marks it as externally implemented, using the `inline` specifier. Use `#CR_IMPL` and `#CR_PIMPL` to implement the coroutine. */
#define CR_EXTERNAL_INLINE inline bool _cr_implementation(); \
};

/** @def COROUTINE(name, inheritance)
	Creates a nested coroutine with the given name.
	Nested coroutines are optimised for deeper nesting, and entering into a callstack has constant complexity instead of linear complexity.
	`inheritance` is the inheritance list of the coroutine.

	For coroutines with templates, use `#TEMPLATE_COROUTINE`. */
#define COROUTINE(name, ...) LIBCR_HELPER_COROUTINE(name, (), private ::cr::CoroutineHelper<name>, ::cr::ExposeCoroutine, ##__VA_ARGS__)
/** @def TEMPLATE_COROUTINE(name, templates, inheritance)
	Creates a templated nested coroutine class.
	`templates` must be the coroutine template arguments, enclosed in parentheses.
	Example:

		template<class T, std::size_t size>
		TEMPLATE_COROUTINE(Array, (T, size)) */
#define TEMPLATE_COROUTINE(name, templates, ...) LIBCR_HELPER_COROUTINE(name, (<LIBCR_HELPER_UNPACK templates>), private ::cr::CoroutineHelper<name<LIBCR_HELPER_UNPACK templates>>, ::cr::ExposeCoroutine, ##__VA_ARGS__)
#define LIBCR_HELPER_COROUTINE(name, templates, ...) \
class name : __VA_ARGS__ \
{ \
	friend class ::cr::CoroutineHelper<name LIBCR_HELPER_UNPACK templates>; \
	typedef name LIBCR_HELPER_UNPACK templates LibCrSelf; \
	typedef ::cr::CoroutineHelper<name LIBCR_HELPER_UNPACK templates> LibCrBase; \
	friend class ::cr::ExposeCoroutine; \
public: \
	using ::cr::Coroutine::operator();

/** @def COROUTINE_PLAIN(name, inheritance)
	Creates a plain coroutine with the given name.
	Plain coroutines are not optimised for nesting, entering into a callstack has linear complexity. However, plain coroutines have less restrictions on their behaviour, and can be used to have multiple "simultaneous" child coroutines, using `#CR_PCALL_NAKED`.
	`inheritance` is the inheritance list of the coroutine.

	For coroutines with templates, use `#TEMPLATE_COROUTINE_PLAIN`. */
#define COROUTINE_PLAIN(name, ...) LIBCR_HELPER_COROUTINE_PLAIN(name, (name), private ::cr::PlainCoroutineHelper<name>, ##__VA_ARGS__)
/** @def TEMPLATE_COROUTINE_PLAIN(name, templates, inheritance)
	Creates a templated plain coroutine class.
	`templates` must be the coroutine template arguments, enclosed in parentheses.
	Example:

		template<class T, std::size_t size>
		TEMPLATE_COROUTINE_PLAIN(Array, (T, size)) */
#define TEMPLATE_COROUTINE_PLAIN(name, templates, ...) LIBCR_HELPER_COROUTINE_PLAIN(name, (<LIBCR_HELPER_UNPACK templates>), private ::cr::PlainCoroutineHelper<name<LIBCR_HELPER_UNPACK templates>>, ##__VA_ARGS__)
#define LIBCR_HELPER_COROUTINE_PLAIN(name, templates, ...) \
class name : __VA_ARGS__ \
{ \
	friend class ::cr::PlainCoroutineHelper<name LIBCR_HELPER_UNPACK templates>; \
	friend class ::cr::MailBoxBase<name LIBCR_HELPER_UNPACK templates>; \
	typedef name LIBCR_HELPER_UNPACK templates LibCrSelf; \
	typedef ::cr::PlainCoroutineHelper<name LIBCR_HELPER_UNPACK templates> LibCrBase; \
public: \
	using ::cr::PlainCoroutineHelper<name LIBCR_HELPER_UNPACK templates>::operator();


/** @def CR_STATE
	Starts the internal / state variable section of a coroutine. */
#define CR_STATE \
private:


#endif