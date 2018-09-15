#ifndef __libcr_sync_mutex_hpp_defined
#define __libcr_sync_mutex_hpp_defined

#include "ConditionVariable.hpp"

namespace cr::sync
{
	/** POD mutex type. */
	class PODMutex
	{
		/** The mutex's condition variable. */
		PODConditionVariable m_cv;
		/** The coroutine that owns the mutex. */
		Coroutine * m_owner;
	public:
		/** Initialises the mutex. */
		void initialise();

		/** Locks the mutex.
			If fails, blocks the coroutine until it acquires the lock. To be used with `#CR_AWAIT`.
		@param[in] self:
			The coroutine trying to acquire the mutex.
		@return
			Whether the lock was acquired. */
		bool lock(
			Coroutine * self);

		/** Tries to lock the mutex.
			Never blocks the coroutine.
		@param[in] self:
			The coroutine trying to acquire the mutex.
		@return
			Whether the lock was acquired. */
		bool try_lock(
			Coroutine * self);

		/** Unlocks the mutex.
			Automatically gives ownership to the first waiting coroutine. */
		void unlock();

		/** Checks for mutex ownership.
		@param[in] self:
			The coroutine to check for.
		@return
			Whether `self` is the current owner of the mutex. */
		inline bool is_owner(
			Coroutine * self);
	};

	/** Mutex type. */
	class Mutex : PODMutex
	{
	public:
		Mutex();

		using PODMutex::lock;
		using PODMutex::try_lock;
		using PODMutex::unlock;
		using PODMutex::is_owner;
	};
}

#include "Mutex.inl"

#endif