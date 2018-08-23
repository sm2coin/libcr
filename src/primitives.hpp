/** @file primitives.hpp
	Contains macros to be used as primitives for coroutines. */
#ifndef __libcr_primitives_hpp_defined
#define __libcr_primitives_hpp_defined

#include "helpermacros.hpp"
#include "specific.hpp"

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
	Calls a nested coroutine from inside another nested coroutine. */
#define CR_CALL(state, ...) LIBCR_HELPER_CALL(__COUNTER__, state, ##__VA_ARGS__)
#define LIBCR_HELPER_CALL(id, ...) do { \
	LIBCR_HELPER_ASSERT_CHILD; \
	LIBCR_HELPER_ASSERT_NESTED("CR_CALL", decltype(LIBCR_HELPER_HEAD(__VA_ARGS__))); \
	LIBCR_HELPER_PREPARE(__VA_ARGS__, NestCoroutineBase::libcr_root, this); \
	NestCoroutineBase::libcr_root->libcr_stack = ExposeNestCoroutineBase::base(&(LIBCR_HELPER_HEAD(__VA_ARGS__))); \
	LIBCR_HELPER_SAVE(id); \
	return (LIBCR_HELPER_HEAD(__VA_ARGS__))(); \
	LIBCR_HELPER_LABEL(id):; \
} while(0)

/** @def CR_PCALL(state, args)
	Calls a plain coroutine from inside another coroutine. Sets a checkpoint before calling, so it behaves like a regular function call. `args` is used to initialise the coroutine. */
#define CR_PCALL(state,...) do { \
			LIBCR_HELPER_ASSERT_PLAIN("CR_CALL_PLAIN", decltype(state)); \
			(state).prepare(__VA_ARGS__); \
			CR_CHECKPOINT; \
			if(!(state)()) \
				return false; \
		} while(0)

/** @def CR_PCALL_NAKED(state, args)
	Calls a plain coroutine from inside another coroutine. Does not set a checkpoint before calling. The coroutine needs to be initialised manually. */
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
			if(is_root()) \
				return true; \
			else \
			{ \
				NestCoroutineBase::libcr_root->libcr_stack = NestCoroutineBase::libcr_stack; \
				return (*NestCoroutineBase::libcr_stack)(); \
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
				m_root->m_stack.child = m_stack.parent; \
				return (*m_stack.parent)(); \
			}
		} while(0); \
	}
#endif

/** @def CR_IMPL(name)
	Starts the external implementation of the given nested coroutine. Must be followed by `#CR_IMPL_END`. */
#define CR_IMPL(name) bool name::_cr_implementation() \
{ \
	LIBCR_HELPER_ASSERT_NESTED_SELF("CR_IMPL"); \
	{ \
		if(m_coroutine_ip) \
			CR_RESTORE; \
		CR_CHECKPOINT; \
	}

/** @def CR_PIMPL(name)
	Starts the external implementation of the given plain coroutine. Must be followed by `#CR_PIMPL_END`. */
#define CR_PIMPL(name) bool name::_cr_implementation() \
{ \
	LIBCR_HELPER_ASSERT_PLAIN_SELF("CR_PIMPL"); \
	{ \
		if(m_coroutine_ip) \
			CR_RESTORE; \
		CR_CHECKPOINT; \
	}

/** @def CR_PINLINE
	Starts the inline implementation of the given plain coroutine. Must be followed by `#CR_PINLINE_END`. */
#define CR_PINLINE private:bool _cr_implementation() \
{ \
	LIBCR_HELPER_ASSERT_PLAIN_SELF("CR_PIMPL"); \
	{ \
		if(m_coroutine_ip) \
			CR_RESTORE; \
		CR_CHECKPOINT; \
	}

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
	if(m_coroutine_ip) \
		CR_RESTORE;

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

/** @def COROUTINE(name)
	Creates a nested coroutine with the given name.
	Nested coroutines are optimised for deeper nesting, and entering into a callstack has constant complexity instead of linear complexity. */
#define COROUTINE(name) \
class name : private ::cr::NestCoroutine<name>, ::cr::ExposeNestCoroutineBase \
{ \
	friend class ::cr::NestCoroutine<name>; \
	typedef name LibCrSelf; \
	typedef ::cr::NestCoroutine<name> Base; \
	friend class ExposeNestCoroutineBase; \
	template<class T, class ...Args> \
	friend void ::cr::prepare(T &, Args&&...); \
public: \
	using Base::operator();

/** @def COROUTINE_PLAIN(name)
	Creates a plain coroutine with the given name.
	Plain coroutines are not optimised for nesting, entering into a callstack has linear complexity. However, plain coroutines have less restrictions on their behaviour, and can be used to have multiple "simultaneous" child coroutines, using `#CR_PCALL_NAKED`. */
#define COROUTINE_PLAIN(name) \
class name : protected ::cr::Coroutine<name> \
{ \
	friend class ::cr::Coroutine<name>; \
	typedef name LibCrSelf; \
public: \
	using ::cr::Coroutine<name>::operator();


/** @def CR_STATE
	Starts the internal / state variable section of a coroutine. */
#define CR_STATE \
private:


#endif