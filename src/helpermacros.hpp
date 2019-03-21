/** @file helpermacros.hpp
	Contains internal helper macros. */
#ifndef __libcr_helpermacros_hpp_defined
#define __libcr_helpermacros_hpp_defined

#include "pp/Assert.hpp"
#include "pp/Overload.hpp"
#include "pp/Arguments.hpp"

#define LIBCR_HELPER_PREPARE(...) LIBCR_HELPER_OVERLOAD_EXISTS(LIBCR_HELPER_PREPARE, __VA_ARGS__)
#define LIBCR_HELPER_PREPARE0() (this)
#define LIBCR_HELPER_PREPARE1(...) (this, __VA_ARGS__)


#define LIBCR_HELPER_UNPACK(...) __VA_ARGS__

#endif