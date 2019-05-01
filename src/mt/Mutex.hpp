/** @file Mutex.hpp
	Contains the thread-safe mutex class. */
#ifndef __libcr_mt_mutex_hpp_defined
#define __libcr_mt_mutex_hpp_defined

#include "Event.hpp"

namespace cr::mt
{
	template<class ConditionVariable>
	/** POD mutex type. */
	class PODMutexPattern : PODConsumableEventPattern<ConditionVariable>
	{
	public:
		/** Tries to lock the mutex.
			Never blocks the coroutine.
		@return
			Whether the lock was acquired. */
		inline bool try_lock();
		/** Locks the mutex.
			If the mutex is locked already, blocks the coroutine until the mutex is released. To be used with `#CR_AWAIT`. */
		inline typename PODConsumableEventPattern<ConditionVariable>::ConsumeCall lock();
		/** Unlocks the mutex.
			The mutex must be locked. Only the owner should unlock the mutex. */
		inline void unlock();

		/** Initialises the mutex to an unlocked state. */
		void initialise();

	};

	template<class ConditionVariable>
	/** Mutex type. */
	class MutexPattern : public PODMutexPattern<ConditionVariable>
	{
		using PODMutexPattern<ConditionVariable>::initialise;
	public:
		inline MutexPattern();
	};

	typedef PODMutexPattern<PODConditionVariable> PODMutex;
	typedef PODMutexPattern<PODFIFOConditionVariable> PODFIFOMutex;
	typedef MutexPattern<PODConditionVariable> Mutex;
	typedef MutexPattern<PODFIFOConditionVariable> FIFOMutex;
}

#include "Mutex.inl"

#endif