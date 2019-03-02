/** @file helpermacros.hpp
	Contains internal helper macros. */
#ifndef __libcr_helpermacros_hpp_defined
#define __libcr_helpermacros_hpp_defined


/** @def LIBCR_DEBUG
	Define this prior to including libcr to activate debugging. */
/** @def LIBCR_RELEASE
	Define this prior to including libcr to deactivate debugging. */

#ifdef LIBCR_RELEASE
#ifdef LIBCR_DEBUG
#error Cannot have LIBCR_RELEASE and LIBCR_DEBUG defined at the same time.
#endif
#endif

#ifdef LIBCR_DEBUG
#ifdef NDEBUG
#undef NDEBUG
#define LIBCR_HAD_NDEBUG
#endif
#endif

#ifdef LIBCR_RELEASE
#ifndef NDEBUG
#define NDEBUG
#define LIBCR_HAD_NO_NDBUG
#endif
#endif

#ifndef NDEBUG
#ifndef LIBCR_DEBUG
#define LIBCR_DEBUG
#endif
#else
#ifndef LIBCR_RELEASE
#define LIBCR_RELEASE
#endif
#endif

#include <cassert>

#ifdef LIBCR_HAD_NDEBUG
#undef LIBCR_HAD_NDEBUG
#define NDEBUG
#endif

#ifdef LIBCR_HAD_NO_NDBUG
#undef LIBCR_HAD_NO_NDBUG
#undef NDEBUG
#endif

#include <type_traits>


#define LIBCR_HELPER_HEAD(head, ...) head
#define LIBCR_HELPER_TAIL(head, ...) __VA_ARGS__
#define LIBCR_HELPER_CAT(a,b) a##b
#define LIBCR_HELPER_BIND(f, args) f args
#define LIBCR_HELPER_BIND2(f, args) f args

#define LIBCR_HELPER_ASSERT_PROTOTHREAD(primitive, type) static_assert(!std::is_base_of<::cr::Coroutine, type>::value, "'" primitive "' can only be used with PROTOTHREADs!")
#define LIBCR_HELPER_ASSERT_COROUTINE(primitive, type) static_assert(std::is_base_of<::cr::Coroutine, type>::value, "'" primitive "' can only be used with COROUTINEs!")
#define LIBCR_HELPER_ASSERT_PROTOTHREAD_SELF(name) LIBCR_HELPER_ASSERT_PROTOTHREAD(name, LibCrSelf)
#define LIBCR_HELPER_ASSERT_COROUTINE_SELF(name) LIBCR_HELPER_ASSERT_COROUTINE(name, LibCrSelf)

#define LIBCR_HELPER_UNPACK(...) __VA_ARGS__

#define LIBCR_HELPER_ARGC_HELPER2(_1, _2, _3, _4, _5, _6, _7, _8, count, ...) count
#define LIBCR_HELPER_ARGC_HELPER1(args) LIBCR_HELPER_ARGC_HELPER2 args
#define LIBCR_HELPER_ARGC(...) LIBCR_HELPER_ARGC_HELPER1((__VA_ARGS__,8,7,6,5,4,3,2,1,0))
#define LIBCR_HELPER_OVERLOAD(base, ...) LIBCR_HELPER_BIND2(LIBCR_HELPER_OVERLOAD_GET_NAME(base, LIBCR_HELPER_ARGC(__VA_ARGS__)), (__VA_ARGS__))
#define LIBCR_HELPER_OVERLOAD_GET_NAME(base, argc) LIBCR_HELPER_BIND(LIBCR_HELPER_CAT, (base, argc))
#define LIBCR_HELPER_MAKE_LIST(...) (__VA_ARGS__)

#define LIBCR_HELPER_PREPARE(...) LIBCR_HELPER_MAKE_LIST(this, ## __VA_ARGS__)

#endif