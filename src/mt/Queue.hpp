/** @file Queue.hpp
	Contains the thread-safe queue types. */
#ifndef __libcr_mt_queue_hpp_defined
#define __libcr_mt_queue_hpp_defined

#include "Semaphore.hpp"
#include "../sync/Queue.hpp"
#include "../util/Atomic.hpp"

namespace cr::mt
{
	template<class T, std::size_t kSize, class Semaphore>
	/** POD fixed queue type.
	@tparam T:
		The queue's element type. */
	class PODFixedQueuePattern : sync::PODQueueBasePattern<Semaphore>
	{
		/** The queue's values. */
		std::array<T, kSize> m_values;
		/** The first element's index. */
		std::atomic_size_t m_start;
		/** The end of the queue. */
		std::atomic_size_t m_end;
	public:
		/** Initialises the queue. */
		void initialise();

		template<class V>
		/** Pushes a value to the queue.
		@tparam V:
			The pushed value's type. */
		TEMPLATE_COROUTINE(PushPattern, (V), void)
		CR_STATE(
			(PODFixedQueuePattern<T, kSize, Semaphore> &) queue,
			(V) value);

			void cr_destroy() {}
		CR_EXTERNAL

		union Push {
			PushPattern<T const&> copy;
			PushPattern<T &&> move;
		};

		/** Pops a value from the queue. */
		COROUTINE(Pop, void)
		CR_STATE(
			(PODFixedQueuePattern<T, kSize, Semaphore> &) queue,
			(T &) target);

			void cr_destroy() {}
		CR_EXTERNAL
	};

	template<std::size_t kSize, class Semaphore>
	class PODFixedQueuePattern<void, kSize, Semaphore> : sync::PODQueueBasePattern<Semaphore>
	{
	public:
		void initialise();

		COROUTINE(Push, void)
		CR_STATE(
			(PODFixedQueuePattern<void, kSize, Semaphore> &) queue);

			void cr_destroy() {}
		CR_EXTERNAL

		COROUTINE(Pop, void)
		CR_STATE(
			(PODFixedQueuePattern<void, kSize, Semaphore> &) queue);

			void cr_destroy() {}
		CR_EXTERNAL
	};

	template<class T, std::size_t kSize, class Semaphore>
	/** Non-POD fixed capacity queue type.
	@tparam T:
		The queue's element type.
	@tparam kSize:
		The queue's capacity.
	@tparam Semaphore:
		The condition variable type to use. */
	class FixedQueuePattern : public PODFixedQueuePattern<T, kSize, Semaphore>
	{
		using PODFixedQueuePattern<T, kSize, Semaphore>::initialise;
	public:
		/** Initialises the queue. */
		inline FixedQueuePattern();
	};

	template<class T, std::size_t kSize>
	using PODFixedQueue = PODFixedQueuePattern<T, kSize, sync::PODSemaphore>;
	template<class T, std::size_t kSize>
	using PODFIFOFixedQueue = PODFixedQueuePattern<T, kSize, sync::PODFIFOSemaphore>;
	template<class T, std::size_t kSize>
	using FixedQueue = FixedQueuePattern<T, kSize, sync::PODSemaphore>;
	template<class T, std::size_t kSize>
	using FIFOFixedQueue = FixedQueuePattern<T, kSize, sync::PODFIFOSemaphore>;
}

#include "Queue.inl"

#endif