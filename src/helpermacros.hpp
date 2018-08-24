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

#define LIBCR_HELPER_CONCAT(a,b) a##b

#define LIBCR_HELPER_LABEL(id) LIBCR_HELPER_CONCAT(libcr_label_,id)

#define LIBCR_HELPER_ASSERT_PLAIN(primitive, type) static_assert(!std::is_base_of<::cr::Coroutine, type>::value, "'" primitive "' can only be used with PLAIN coroutines!")
#define LIBCR_HELPER_ASSERT_NESTED(primitive, type) static_assert(std::is_base_of<::cr::Coroutine, type>::value, "'" primitive "' can only be used with NESTED coroutines!")
#define LIBCR_HELPER_ASSERT_PLAIN_SELF(name) LIBCR_HELPER_ASSERT_PLAIN(name, LibCrSelf)
#define LIBCR_HELPER_ASSERT_NESTED_SELF(name) LIBCR_HELPER_ASSERT_NESTED(name, LibCrSelf)

#define LIBCR_HELPER_UNPACK(...) __VA_ARGS__

#define LIBCR_HELPER_PREPARE(obj, ...) (obj).prepare(__VA_ARGS__)

#define LIBCR_HELPER_ASSERT_CHILD do { \
	LIBCR_HELPER_ASSERT_NESTED_SELF("LIBCR_HELPER_ASSERT_CHILD"); \
	assert("The coroutine must be the child." && Coroutine::libcr_root->libcr_stack == this); \
} while(0)

#endif