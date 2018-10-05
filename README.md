# libcr

> Coroutine library draft.
> libcr is intended to create minimal overhead coroutines with basic functionality.

## Features

Libcr is a bare-bones **stack-less** coroutine implementation, extremely lightweight and efficient, with no fancy features. However, it is well suited to use as a base to create more powerful coroutine primitives. Currently, libcr features the following:
 * Plain coroutines: with only one pointer as state, these coroutines are extremely low in overhead.
 * Nest-coroutines: with 5 pointers as state, these coroutines allow constant-complexity (O(1)) resuming of coroutines of arbitrary nesting depth, and support scheduling, with constant-complexity enqueueing and removing of coroutines from a waiting list.
 * Extreme efficiency: Saving a coroutine's context only requires updating a single pointer. Likewise, the resuming of a coroutine's execution only requires reading that pointer, and making a call.
 * All coroutines are POD types per default, allowing them to be put into `union` types for better memory usage.
 * Primitives:
 	* A blocking await primitive (`#CR_CALL`), that allows waiting for a child coroutine to finish.
 	* A yield primitive (`#CR_YIELD`), that allows yielding the coroutine execution.
 	* A return primitive (`#CR_RETURN`), that ends the coroutine.
 	* A blocking wait primitive (`#CR_AWAIT`), that can be used to wait for blocking operations.
 	* A checkpoint primitive (`#CR_CHECKPOINT`) that saves the coroutine execution state for the next time it is resumed.
 * Clean, easy to understand syntax.
 * No external libraries or kernel functions are used.
 * Synchronisation primitives such as `mutex`, `condition variable`, `event`, `barrier`, `promise` and `future` (namespace `cr::sync`).


## Compiling

You need to have `CMake` installed. Navigate to the `libcr` directory, and execute:

	cmake .

This will generate makefiles or project files (depending on your setup), and also create an include directory containing all headers and the license, in `libcr/include`.

## Documentation

If you have `doxygen` installed, navigate to the `libcr` directory, and execute:

	doxygen Doxyfile

This generates documentation in `libcr/html`.

## License

A copy of the license can be found in `libcr/LICENSE`.
libcr is licensed under the GNU Affero General Public License, version 3.

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
Then, `#CR_STATE` is followed by the local variables of the function.
Then, use `#CR_INLINE` for an inline coroutine definition, or `#CR_EXTERNAL`, to mark the coroutine as externally implemented.

More information can be extracted from the documentation of the classes and macros.