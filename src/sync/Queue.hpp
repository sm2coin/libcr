/** @file Queue.hpp
	Contains the thread-unsafe queue types. */
#ifndef __libcr_sync_queue_hpp_defined
#define __libcr_sync_queue_hpp_defined

#include "Semaphore.hpp"
#include "../primitives.hpp"

namespace cr::sync
{
	template<class Semaphore>
	/** POD queue base type. */
	class PODQueueBasePattern
	{
		/** The currently used slots. */
		Semaphore m_elements;
		/** The currently free slots.*/
		Semaphore m_free;
	public:
		/** Initialises the queue.
		@param[in] capacity:
			The capacity the queue should have.  */
		void initialise(
			std::size_t capacity);

		/** Waits until there is at least one free slot in the queue.
			To be used with `#CR_AWAIT`. */
		[[nodiscard]] constexpr typename Semaphore::WaitCall free();
		/** Waits until there is at least one element in the queue.
			To be used with `#CR_AWAIT`. */
		[[nodiscard]] constexpr typename Semaphore::WaitCall elements();

		/** Adds an element to the queue, notifying `elements()`.
			This has to be called after the element is added. */
		inline void push();
		/** Removes an element from the queue, notifying `free()`.
			This has to be called after the element is removed. */
		inline void pop();
	};

	template<class T, std::size_t kSize, class Semaphore>
	/** POD fixed capacity queue type.
	@tparam T:
		The queue's data type.
	@tparam kSize:
		The queue's capacity.
	@tparam Semaphore:
		The condition variable type to use internally. */
	class PODFixedQueuePattern : PODQueueBasePattern<Semaphore>
	{
		/** The queue's values. */
		std::array<T, kSize> m_values;
		/** The first element's index. */
		std::size_t m_start;
		/** The end of the queue. */
		std::size_t m_end;
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
	class PODFixedQueuePattern<void, kSize, Semaphore> : PODQueueBasePattern<Semaphore>
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
		The semaphore type to use. */
	class FixedQueuePattern : public PODFixedQueuePattern<T, kSize, Semaphore>
	{
		using PODFixedQueuePattern<T, kSize, Semaphore>::initialise;
	public:
		/** Initialises the queue. */
		inline FixedQueuePattern();
	};

	template<class T, std::size_t kSize>
	/** POD fixed queue type.
	@tparam T:
		The queue's data type.
	@tparam kSize:
		The queue's capacity. */
	using PODFixedQueue = PODFixedQueuePattern<T, kSize, sync::PODSemaphore>;
	template<class T, std::size_t kSize>
	/** POD fixed queue type with FIFO notifications.
	@tparam T:
		The queue's data type.
	@tparam kSize:
		The queue's capacity. */
	using PODFIFOFixedQueue = PODFixedQueuePattern<T, kSize, sync::PODFIFOSemaphore>;
	template<class T, std::size_t kSize>
	/** Fixed queue type.
	@tparam T:
		The queue's data type.
	@tparam kSize:
		The queue's capacity. */
	using FixedQueue = FixedQueuePattern<T, kSize, sync::PODSemaphore>;
	template<class T, std::size_t kSize>
	/** Fixed queue type with FIFO notifications.
	@tparam T:
		The queue's data type.
	@tparam kSize:
		The queue's capacity. */
	using FIFOFixedQueue = FixedQueuePattern<T, kSize, sync::PODFIFOSemaphore>;
}

#include "Queue.inl"

#endif