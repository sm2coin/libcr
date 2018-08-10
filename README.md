# coroutines

Coroutine library draft.

## Introduction

A coroutine is a function whose execution can be stopped and continued at will.
This is especially useful for functions that wait for input, or for generators.

### The coroutine model

In this library, a coroutine is modelled as a machine with an external interface, for inputs and outputs.
The coroutine can be called to advance the execution until the next break point (yield).
The coroutine always returns whether its execution finished.
Calling a finished coroutine does nothing.

A coroutine has no local variables, they must be part of the coroutine state.


## Coding

### Example coroutine

```c++
// Countdown coroutine.
COROUTINE(Count)
	// In this section, the external interface of the coroutine is described.
	// You can add getters, setters, variables, and even constructors.

	/** Retrieves the counter. */
	inline std::size_t counter() const;
	/** Initalises the counter execution.
	@param[in] start:
		Where to start counting. */
	Count(
		std::size_t start);
CR_STATE
	// In this section, the internal state of the coroutine is declared.
	std::size_t m_counter;
// Inline coroutine definition.
CR_INL_BEGIN
	// In this section, the coroutine code is written.
	while(m_counter)
	{
		// Yields the execution, in the next invocation, it continues where it left off.
		CR_YIELD;

		m_counter--;
	}

	// Yield the 0, too.
	CR_YIELD;
CR_INL_END

// The coroutine can be accessed like any class object.
std::size_t Count::counter() const
{
	return m_counter;
}

Count::Count(
	std::size_t start):
	m_counter(start)
{
}
```

The coroutine can be used as follows:

```c++
// Instantiate the routine execution.
Count count(42);

// Execute until it is finished.
while(!count())
{
	// Retrieve the counter value.
	std::cout << count.counter() << std::endl;
}
```

* To create a subroutine named `name`, the `COROUTINE(name)` macro is used.
* To add local variables / state to a coroutine, the `CR_STATE` macro has to be used.
* The `CR_INL_BEGIN`, macros are used to mark the start of the actual code of the coroutine.
* `CR_INL_END` signifies the end of the coroutine code and of the coroutine definition.
	* `CR_INL_END_NORETURN` can be used instead, if there is no control flow that reaches the end of the function.
	This decreases the code size.
* The section order is not flexible, but `CR_STATE` is optional.
* The coroutine may be defined outside its declaration via `CR_EXTERNAL` instead of `CR_INL_BEGIN`.
 The coroutine is implemented via `CR_IMPL_BEGIN(name)` and `CR_IMPL_END`.

### Nested coroutines

If a coroutine is nested inside another coroutine, then the inner coroutine execution has to be part of the callin coroutine's state.
Then, the macro `CR_CALL(cr_name)` can be used to call the other coroutine. *Note, that the coroutine object needs to be initialised with arguments before calling.*
The control flow only resumes after the called coroutine is finished.

```c++
COROUTINE(Inner)
	Inner(
		std::size_t count):
		count(count)
	{
	}
CR_STATE
	std::size_t i;
	std::size_t count;
CR_EXTERNAL

COROUTINE(Outer)
CR_STATE
	Inner m_inner;
CR_INL_BEGIN
	m_inner = Inner(42);
	// Execute Inner(42).
	CR_CALL(m_inner);
CR_INL_END


// External implementation.
CR_IMPL_BEGIN(Inner)
	for(i = 0; i < count; i++)
		CR_YIELD;
CR_IMPL_END

```