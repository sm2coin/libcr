#ifndef __libcr_sync_sync_hpp_defined
#define __libcr_sync_sync_hpp_defined

#include "Barrier.hpp"
#include "ConditionVariable.hpp"
#include "Event.hpp"
#include "Future.hpp"
#include "Mutex.hpp"
#include "Promise.hpp"

/** Contains all synchronisation primitives.
	These primitives are not thread-safe, however. For thread-safe versions, see namespace `cr::mt`. */
namespace cr::sync
{
}

#endif