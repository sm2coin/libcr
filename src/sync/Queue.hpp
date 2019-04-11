#ifndef __libcr_sync_queue_hpp_defined
#define __libcr_sync_queue_hpp_defined

#include "Semaphore.hpp"
#include "../primitives.hpp"

namespace cr::sync
{
	template<class ConditionVariable>
	/** POD fixed capacity queue base type. */
	class PODFixedQueueBasePattern
	{
		typedef PODSemaphoreBase<ConditionVariable> Semaphore;

		Semaphore m_elements;
		Semaphore m_free;
	public:
		void initialise(
			std::size_t capacity);

		[[nodiscard]] constexpr typename Semaphore::WaitCall free();
		[[nodiscard]] constexpr typename Semaphore::WaitCall elements();

		inline void push();
		inline void pop();
	};

	template<class T, std::size_t kSize, class ConditionVariable>
	/** POD fixed capacity queue type.
	@tparam T:
		The queue's data type.
	@tparam kSize:
		The queue's capacity.
	@tparam ConditionVariable:
		The condition variable type to use internally. */
	class PODFixedQueuePattern : PODFixedQueueBasePattern<ConditionVariable>
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
			(PODFixedQueuePattern<T, kSize, ConditionVariable> &) queue,
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
			(PODFixedQueuePattern<T, kSize, ConditionVariable> &) queue,
			(T &) target);

			void cr_destroy() {}
		CR_EXTERNAL
	};

	template<std::size_t kSize, class ConditionVariable>
	class PODFixedQueuePattern<void, kSize, ConditionVariable> : PODFixedQueueBasePattern<ConditionVariable>
	{
	public:
		void initialise();

		COROUTINE(Push, void)
		CR_STATE(
			(PODFixedQueuePattern<void, kSize, ConditionVariable> &) queue);

			void cr_destroy() {}
		CR_EXTERNAL

		COROUTINE(Pop, void)
		CR_STATE(
			(PODFixedQueuePattern<void, kSize, ConditionVariable> &) queue);

			void cr_destroy() {}
		CR_EXTERNAL
	};

	template<class T, std::size_t kSize, class ConditionVariable>
	/** Non-POD fixed capacity queue type.
	@tparam T:
		The queue's element type.
	@tparam kSize:
		The queue's capacity.
	@tparam ConditionVariable:
		The condition variable type to use. */
	class FixedQueuePattern : public PODFixedQueuePattern<T, kSize, ConditionVariable>
	{
		using PODFixedQueuePattern<T, kSize, ConditionVariable>::initialise;
	public:
		/** Initialises the queue. */
		inline FixedQueuePattern();
	};

	template<class T, std::size_t kSize>
	using PODFixedQueue = PODFixedQueuePattern<T, kSize, sync::PODConditionVariable>;
	template<class T, std::size_t kSize>
	using PODFIFOFixedQueue = PODFixedQueuePattern<T, kSize, sync::PODFIFOConditionVariable>;
	template<class T, std::size_t kSize>
	using FixedQueue = FixedQueuePattern<T, kSize, sync::PODConditionVariable>;
	template<class T, std::size_t kSize>
	using FIFOFixedQueue = FixedQueuePattern<T, kSize, sync::PODFIFOConditionVariable>;
}

#include "Queue.inl"

#endif