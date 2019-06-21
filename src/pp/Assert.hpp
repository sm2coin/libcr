/** @file Assert.hpp
	Contains macros related to release/debug modes and assertions. */
#ifndef __libcr_pp_assert_hpp_defined
#define __libcr_pp_assert_hpp_defined

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

// ASSERTS

#define LIBCR_HELPER_ASSERT_PROTOTHREAD(primitive, type) static_assert(!std::is_base_of<::cr::Coroutine, std::decay_t<type>>::value, "'" primitive "' can only be used with PROTOTHREADs!")
#define LIBCR_HELPER_ASSERT_COROUTINE(primitive, type) static_assert(std::is_base_of<::cr::Coroutine, std::decay_t<type>>::value, "'" primitive "' can only be used with COROUTINEs!")
#define LIBCR_HELPER_ASSERT_PROTOTHREAD_SELF(name) LIBCR_HELPER_ASSERT_PROTOTHREAD(name, LibCrSelf)
#define LIBCR_HELPER_ASSERT_COROUTINE_SELF(name) LIBCR_HELPER_ASSERT_COROUTINE(name, LibCrSelf)

#endif