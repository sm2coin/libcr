# libcr

> libcr offers extremely lightweight massive 1:N and M:N multitasking via coroutines and protothreads.

*libcr is still evolving, from time to time, breaking changes are made, to improve the final product.
Constructive criticism and suggestions are very much appreciated.*

For a quick impression of how to program using libcr, read [the intro](docs/intro.md).

**Coroutines?**&ensp;
Coroutines, in contrast to functions, can terminate ("yield") at any point, and be resumed later.
This is similar to a thread that sleeps or blocks, but without all the performance and resource overhead of real threads.
They are [non-preemptive](https://en.wikipedia.org/wiki/Cooperative_multitasking "Cooperative multitasking on Wikipedia") in nature, so they do not achieve true parallelism on their own.

**Protothreads?**&ensp;
[Protothreads](https://en.wikipedia.org/wiki/Protothread "Protothread on Wikipedia") are a simplified form of coroutine that can only yield within the top-level context (they cannot yield from within nested calls).
Protothreads lack many features, but are very efficient: one protothread only has an overhead of 8 bytes of memory (4 on 32-bit systems), and a task switch is a simple call.
The initial implementation of protothreads was based on [Duff's device](https://en.wikipedia.org/wiki/Duff%27s_device "Duff's device on Wikipedia"), which had the drawback that it forbids the use of `switch` statements inside the function.
In libcr, a GCC-specific workaround has been found, which allows for restriction-free programming.

**License**&ensp;
The library is released under the **GNU Affero General Public License, version 3** (AGPLv3).
A copy of the license text can be found in the file `LICENSE`.

## 1. Features

libcr is intended to be used on resource-constrained devices, such as Raspberry Pis.
This means that everything is designed with minimum memory usage and maximum efficiency in mind.

### 1.1. Technical

**Lightweight**&ensp;
A single coroutine takes up 56 bytes (on a 64-bit platform, in release mode) of memory, and task switches are much cheaper than kernel thread task switches.
This allows for (more or less) massive parallelism even on resource-constrained systems.

**Thread-safe**&ensp;
The library is thread-safe; While coroutines are usually implemented for single-threaded contexts, libcr allows true [M:N threading](https://en.wikipedia.org/wiki/Thread_(computing)#M:N_.28hybrid_threading.29).
Coroutines can be passed between threads without any issues.
This means that even greater multitasking can be achieved when using libcr with real threads.
Of course, libcr also has a thread-unsafe implementation for even faster task switching on single-threaded systems or when it is clear that every coroutine will stay in the thread it was created in.

**Speed**&ensp;
According to benchmarks performed on an Intel(R) Core(TM) i7-8700 CPU (3.20GHz), task switching with coroutines is around 125 times faster than kernel task switches when running in a single thread (we measured 50MHz [20ns] for thread-unsafe task switches for coroutines, and around 400kHz (2.5µs) for kernel task switches [thread synchronisations] on the same machine).
We also measured 17MHz (59ns) for thread-safe coroutine context switches in a single thread on the same machine.
Since coroutines can run in multiple threads, this speed scales with the number of cores.

**No dynamic allocations**&ensp;
In libcr, nothing is allocated dynamically, which gives the user maximum freedom to control the performance.
This also means that no stack is allocated for coroutines, which allows libcr to have small memory footprint per task.

**Operating system independent**&ensp;
Although GCC-specific features are used (the `&&` and `goto *` operators), no operating system specific functionality is used anywhere in the library, so libcr can be used on any system GCC supports.

**Plain old data (POD) types**&ensp;
The library fully supports POD types for everything, so that the user can optimise the code using libcr to its limits.
This allows coroutines that are called sequentially to be put into unions to save memory.
Of course, RAII versions of all types are provided, as well, so that safer code can be written more easily.

**Task-local storage**&ensp;
Similar to `thread_local` storage, libcr supports storage that is only accessible from a single coroutine and all its children.
This allows the efficient reuse of memory for things like `itoa` buffers and more.

### 1.2. Programming

**Exceptions**&ensp;
Implementing exceptions in coroutines is not easy, as native exception mechanisms are not compatible with the coroutine execution model.
However, we still managed to implement a working, minimalistic exception model.
Exceptions are of the form *error* / *no error*, with no values passed, which makes them very efficient.

**Events**&ensp;
libcr implements a 100% event-driven execution model, so no polling or similar things are needed.
Coroutine-aware versions of the most common synchronisation primitives (such as condition variables, mutexes, barriers, futures/promises, etc.) are provided by libcr.
Those can be used just like their counterparts for threads, but are much faster due to the performant synchronisation / task switching of coroutines.

**Syntax**&ensp;
Since coroutines are not natively supported (ignoring the coroutine TS), extensive use of preprocessor macros is needed to emulate them in a user-friendly way.
Due to constraints of the preprocessor, the syntax is not optimal, but readable enough so that the code seems somewhat natural and intuitive.
For a quick impression of how to program using libcr, read [the intro](docs/intro.md).

## 2. Installation

Make sure to install some reasonably recent version of CMake.
Then navigate into the repository and execute

	cmake .

This generates an include directory that can be used to integrate libcr into other projects, as well as make / project files that build the library (depends on how your system is configured).

For release mode, execute:

	cmake . -DLIBCR_RELEASE=ON

## 3. Documentation

You can extract the documentation of the code using doxygen.
A doxygen configuration file is part of the repository.
It makes doxygen output the documentation in `libcr/html/`.

## 4. Testing and Benchmarks

You can use [libcr-test](https://github.com/sm2coin/libcr-test "libcr-test on Github") to test and benchmark libcr.
