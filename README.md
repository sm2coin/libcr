# coroutines

Coroutine library draft.

## Compiling

You need to have `CMake` installed. Navigate to the `libcr` directory, and execute:

	cmake .

This will generate makefiles or project files (depending on your setup), and also create an include directory containing all headers and the license, in `libcr/include`.

## Introduction

A coroutine is a function whose execution can be stopped and continued at will.
This is especially useful for functions that wait for input, or for generators.

### The coroutine model

In this library, a coroutine is modelled as a machine with an external interface, for inputs and outputs.
The coroutine can be called to advance the execution until the next break point (yield).
The coroutine always returns whether its execution finished.
Calling an already finished coroutine is forbidden.

A coroutine's implementation cannot have local variables, they must be part of the coroutine state.

#### Nested coroutines

This type of coroutine is optimised for nesting, with constant complexity entry into a coroutine, regardless of nesting depth. Nested coroutines can only have one child coroutine executing at any time.

#### Plain coroutines

Plain coroutines are very basic coroutines without optimisation, but can be used for tricks such as multiple parallel child coroutine executions within the same calling coroutine.

### Tutorial

Create a nested coroutine with `COROUTINE(name)` or a plain coroutine with `COROUTINE_PLAIN(name)`.
Following that comes the public interface of the coroutine, such as paramters or functions.
Then, `CR_STATE` is followed by the local variables of the function.
Then, use `CR_INLINE` for an inline coroutine definition, or `CR_EXTERNAL`, to mark the coroutine as externally implemented.

More information can be extracted from the documentation of the classes and macros.