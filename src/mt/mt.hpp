/** @file mt.hpp
	Includes all headers in the cr::mt namespace. */
#ifndef __libcr_mt_mt_hpp_defined
#define __libcr_mt_mt_hpp_defined

#include "Barrier.hpp"
#include "ConditionVariable.hpp"
#include "Event.hpp"
#include "Future.hpp"
#include "Mutex.hpp"
#include "Promise.hpp"

/** Contains all synchronisation primitives.
	These primitives are thread-safe. For thread-unsafe versions, see namespace `cr::sync`. */
namespace cr::mt
{
}

#endif