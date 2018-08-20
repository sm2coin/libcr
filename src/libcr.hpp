#ifndef __coroutines_coroutines_hpp_defined
#define __coroutines_coroutines_hpp_defined

#include <cstddef>
#include <utility>

namespace cr
{
	/** Basic coroutine state. */
	class Coroutine
	{
	public:
		Coroutine() = default;
	protected:
		/** Contains the last saved line of execution. */
		std::size_t m_coroutine_line;

		/** Prepares the coroutine for execution. */
		void prepare();
	};
}

/** @def COROUTINE(name)
	Starts a coroutine declaration.
	Also starts the public interface declaration. */
#define COROUTINE(name) \
class name : public ::cr::Coroutine \
{ \
public:

/** @def CR_EXTERNAL(args)
	Ends a coroutine declaration, and marks it as externally implemented.
	`args` are arguments that are to be supplied to the coroutine on every invokation. */
#define CR_EXTERNAL(...) \
public: \
	bool operator()(__VA_ARGS__); \
};

/** @def CR_STATE
	Starts the internal / state variable section of a coroutine. */
#define CR_STATE \
private:

/** @def CR_IMPL_BEGIN(name, args)
	Begins an external implementation of a coroutine.
	`args` are arguments that are to be supplied to the coroutine on every invokation. */
#define CR_IMPL_BEGIN(name, ...) \
	bool name::operator()(__VA_ARGS__) \
	{ \
		switch(::cr::Coroutine::m_coroutine_line) \
		{ \
		default: \
		case 0:

/** @def CR_INL_BEGIN(args)
	Marks the beginning of an inline coroutine implementation.
	`args` are arguments that are to be supplied to the coroutine on every invokation. */
#define CR_INL_BEGIN(...) \
public: \
	inline bool operator()(__VA_ARGS__) \
	{ \
		switch(::cr::Coroutine::m_coroutine_line) \
		{ \
		default: \
		case 0:

/** @def CR_CHECKPOINT
	Saves the current execution as entry point for the next invokation. */
// Add [[fallthrough]] in C++17.
#if __cplusplus > 201402L
#define CR_CHECKPOINT do { \
			::cr::Coroutine::m_coroutine_line = __LINE__; \
			[[fallthrough]]; \
			case __LINE__:; \
		} while(0)
#else
#define CR_CHECKPOINT do { \
			::cr::Coroutine::m_coroutine_line = __LINE__; \
			case __LINE__:; \
		} while(0)
#endif
/** @def CR_RESTORE
	Yields and continues execution at the last checkpoint. */
#define CR_RESTORE do { \
			return false; \
		} while(0)

// uses https://de.wikipedia.org/wiki/Duff%E2%80%99s_Device
// to jump directly back into the code (e.g. the while loop)

/** @def CR_YIELD
	Halts the coroutine's execution until its next invokation. */
#define CR_YIELD do {\
			::cr::Coroutine::m_coroutine_line = __LINE__; \
			return false; \
			case __LINE__:; \
		} while(0)

/** @def CR_RETURN
	Terminates the coroutine and marks it as executed. */
#define CR_RETURN do { \
			CR_CHECKPOINT; \
			return true; \
		} while(0)

/** @def CR_CALL(state, args)
	Calls a coroutine from inside another coroutine.
	Sets a checkpoint before calling, so it behaves like a regular function call.
	The coroutine needs to be initialised manually.
	`args` are arguments that are to be supplied to the coroutine on every invokation (not initialisation). */
#define CR_CALL(state, ...) do { \
			CR_CHECKPOINT; \
			if(!state(__VA_ARGS__)) \
				return false; \
		} while(0)

/** @def CR_CALL_NAKED(state, args)
	Calls a coroutine from inside another coroutine.
	Does not set a checkpoint before calling.
	The coroutine needs to be initialised manually.
	`args` are arguments that are to be supplied to the coroutine on every invokation (not initialisation). */
#define CR_CALL_NAKED(state, ...) do { \
			if(!state(__VA_ARGS__)) \
				return false; \
		} while(0)


/** @def CR_INL_END_NORETURN
	Ends a coroutine declaration and its inline definition.
	Special use for functions whose control flow does not reach the end of the function body to save an implicit return. */
#define CR_INL_END_NORETURN \
		} \
	} \
};

/** @def @CR_IMPL_END_NORETURN
	Ends a coroutine's external implementation.
	Special use for functions whose control flow does not reach the end of the function body to save an implicit return. */
#define CR_IMPL_END_NORETURN \
		} \
	}



/** @def CR_INL_END
	Ends a coroutine declaration and its inline definition.
	Contains an implicit return statement. */
#define CR_INL_END \
		CR_RETURN; \
CR_INL_END_NORETURN

/** @def CR_IMPL_END
	Ends a coroutine's external implementation.
	Contains an implicit return statement. */
#define CR_IMPL_END \
		CR_RETURN; \
CR_IMPL_END_NORETURN

#endif