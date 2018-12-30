#ifndef __libcr_mt_detail_softmutex_hpp_defined
#define __libcr_mt_detail_softmutex_hpp_defined

#include <atomic>

namespace cr::mt::detail
{
	/** POD userspace mutex (not coroutine-aware). */
	class PODSoftMutex
	{
		/** The mutex's locking flag. */
		std::atomic_bool m_locked;
	public:
		/** Initialises the mutex. */
		inline void initialise();
		/** Locks the mutex. */
		void lock();
		/** Tries once to lock the mutex.
		@return
			Whether the mutex was successfully locked. */
		bool try_lock();
		/** Unlocks the mutex. */
		void unlock();
	};

	/** Userspace mutex (not coroutine-aware). */
	class SoftMutex : public PODSoftMutex
	{
	public:
		/** Initialises the mutex. */
		inline SoftMutex();

		using PODSoftMutex::lock;
		using PODSoftMutex::unlock;
	};

	/** Userspace mutex lock guard. */
	class LockGuard
	{
		/** The locked mutex. */
		PODSoftMutex *m_mutex;
	public:
		/** Creates a lock guard without any mutex. */
		inline LockGuard();
		/** Creates a lock guard locking a mutex.
		@param[in] mutex:
			The mutex to lock. */
		explicit inline LockGuard(
			PODSoftMutex &mutex);
		/** Unlocks the mutex, if any. */
		~LockGuard();

		/** Unlocks the locked mutex.
			The lock guard must have locked a mutex. */
		inline void unlock();
		/** Locks a mutex.
			The lock guard must not have locked a mutex. */
		inline void lock(
			SoftMutex &mutex);

		inline bool try_lock(
			SoftMutex &mutex);

		/** Whether the lock guard locks a mutex. */
		inline bool locked() const;
	};
}

#include "SoftMutex.inl"

#endif