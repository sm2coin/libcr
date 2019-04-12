namespace cr::mt
{
	namespace detail
	{
		template<class Values>
		std::size_t inc_wrap(
			Values const& values,
			std::atomic_size_t &value)
		{
			std::size_t index = value.load(std::memory_order_relaxed);
			std::size_t next;
			// Increment with wrap-around.
			do {
				if(index == values.size() -1)
					next = 0;
				else
					next = index + 1;
			} while(!value.compare_exchange_weak(
				index,
				next,
				std::memory_order_relaxed,
				std::memory_order_relaxed));

			return index;
		}
	}

	template<class T, std::size_t kSize, class Semaphore>
	void PODFixedQueuePattern<T, kSize, Semaphore>::initialise()
	{
		sync::PODQueueBasePattern<Semaphore>::initialise(kSize);
		std::atomic_init(&m_start, (std::size_t) 0);
		std::atomic_init(&m_end, (std::size_t) 0);
	}

	template<class T, std::size_t kSize, class Semaphore>
	template<class V>
	CR_IMPL(PODFixedQueuePattern<T, kSize, Semaphore>::PushPattern<V>)
		CR_AWAIT(queue->free());
		util::assign(
			queue->m_values[detail::inc_wrap(queue->m_values, queue->m_end)],
			std::forward<V>(value));
		queue->push();
	CR_IMPL_END

	template<class T, std::size_t kSize, class Semaphore>
	CR_IMPL(PODFixedQueuePattern<T, kSize, Semaphore>::Pop)
		CR_AWAIT(queue->elements());
		util::assign(
			*target,
			std::move(
				queue->m_values[detail::inc_wrap(queue->m_values, queue->m_start)]));
		queue->pop();
	CR_IMPL_END

	template<std::size_t kSize, class Semaphore>
	void PODFixedQueuePattern<void, kSize, Semaphore>::initialise()
	{
		sync::PODQueueBasePattern<Semaphore>::initialise(kSize);
	}

	template<std::size_t kSize, class Semaphore>
	CR_IMPL(PODFixedQueuePattern<void, kSize, Semaphore>::Push)
		CR_AWAIT(queue->free());
		queue->push();
	CR_IMPL_END

	template<std::size_t kSize, class Semaphore>
	CR_IMPL(PODFixedQueuePattern<void, kSize, Semaphore>::Pop)
		CR_AWAIT(queue->elements());
		queue->pop();
	CR_IMPL_END

	template<class T, std::size_t kSize, class Semaphore>
	FixedQueuePattern<T, kSize, Semaphore>::FixedQueuePattern()
	{
		initialise();
	}
}