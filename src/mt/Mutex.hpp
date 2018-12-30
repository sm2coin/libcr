/** @file Mutex.hpp
	Contains the mutex class. */
#ifndef __libcr_mt_mutex_hpp_defined
#define __libcr_mt_mutex_hpp_defined

#include "Event.hpp"

namespace cr::mt
{
	template<class ConditionVariable>
	/** POD mutex type. */
	class PODMutexBase : PODConsumableEventBase<ConditionVariable>
	{
	public:
		/** Tries to lock the mutex.
			Never blocks the coroutine.
		@return
			Whether the lock was acquired. */
		using try_lock = PODConsumableEventBase<ConditionVariable>::try_consume;
		using lock = PODConsumableEventBase<ConditionVariable>::consume;
		using unlock = PODConsumableEventBase<ConditionVariable>::fire;

		/** Initialises the mutex to an unlocked state. */
		inline void initialise();

	};

	template<class ConditionVariable>
	/** Mutex type. */
	class MutexBase : PODMutexBase<ConditionVariable>
	{
	public:
		inline Mutex();

		using PODMutexBase<ConditionVariable>::lock;
		using PODMutexBase<ConditionVariable>::try_lock;
		using PODMutexBase<ConditionVariable>::unlock;
	};

	typedef PODMutexBase<PODConditionVariable> PODMutex;
	typedef PODMutexBase<PODFIFOConditionVariable> PODFIFOMutex;
	typedef MutexBase<PODConditionVariable> Mutex;
	typedef MutexBase<PODFIFOConditionVariable> FIFOMutex;
}

#endif