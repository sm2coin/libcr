/** @file Mutex.hpp
	Contains the thread-safe mutex class. */
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
		inline bool try_lock();
		/** Locks the mutex.
			If the mutex is locked already, blocks the coroutine until the mutex is released. To be used with `#CR_AWAIT`. */
		inline typename PODConsumableEventBase<ConditionVariable>::ConsumeCall lock();
		/** Unlocks the mutex.
			The mutex must be locked. Only the owner should unlock the mutex. */
		inline void unlock();

		/** Initialises the mutex to an unlocked state. */
		void initialise();

	};

	template<class ConditionVariable>
	/** Mutex type. */
	class MutexBase : public PODMutexBase<ConditionVariable>
	{
		using PODMutexBase<ConditionVariable>::initialise;
	public:
		inline MutexBase();
	};

	typedef PODMutexBase<PODConditionVariable> PODMutex;
	typedef PODMutexBase<PODFIFOConditionVariable> PODFIFOMutex;
	typedef MutexBase<PODConditionVariable> Mutex;
	typedef MutexBase<PODFIFOConditionVariable> FIFOMutex;
}

#include "Mutex.inl"

#endif