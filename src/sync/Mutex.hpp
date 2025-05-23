/** @file Mutex.hpp
	Contains the mutex class. */
#ifndef __libcr_sync_mutex_hpp_defined
#define __libcr_sync_mutex_hpp_defined

#ifdef LIBCR_INLINE
#define __LIBCR_INLINE inline
#else
#define __LIBCR_INLINE
#endif

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
		__LIBCR_INLINE void initialise();

		/** Helper type that simplifies usage of `lock()` with `#CR_AWAIT`. */
		class LockCall
		{
			/** The mutex to lock. */
			PODMutex &m_mutex;
		public:
			/** Creates a lock call.
			@param[in] mutex:
				The mutex to lock. */
			constexpr LockCall(
				PODMutex &mutex);

			/** Locks the mutex.
			@param[in] self:
				The coroutine locking the mutex.
			@return
				Whether the mutex was successfully locked. */
			__LIBCR_INLINE mayblock libcr_wait(
				Coroutine * self);
		};

		/** Locks the mutex.
			To be used with `#CR_AWAIT`. */
		[[nodiscard]] constexpr LockCall lock();

		/** Tries to lock the mutex.
			Never blocks the coroutine.
		@param[in] self:
			The coroutine trying to acquire the mutex.
		@return
			Whether the lock was acquired. */
		__LIBCR_INLINE bool try_lock(
			Coroutine * self);

		/** Unlocks the mutex.
			Automatically gives ownership to the first waiting coroutine. */
		__LIBCR_INLINE void unlock();

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
		__LIBCR_INLINE Mutex();

		using PODMutex::lock;
		using PODMutex::try_lock;
		using PODMutex::unlock;
		using PODMutex::is_owner;
	};
}

#include "Mutex.inl"

#endif