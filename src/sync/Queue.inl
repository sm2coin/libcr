namespace cr::sync
{

	template<class Semaphore>
	void PODQueueBasePattern<Semaphore>::initialise(
		std::size_t capacity)
	{
		assert(capacity > 0);
		m_elements.initialise(0);
		m_free.initialise(capacity);
	}

	template<class Semaphore>
	constexpr typename Semaphore::WaitCall PODQueueBasePattern<Semaphore>::free()
	{
		return m_free.wait();
	}

	template<class Semaphore>
	constexpr typename Semaphore::WaitCall PODQueueBasePattern<Semaphore>::elements()
	{
		return m_elements.wait();
	}

	template<class Semaphore>
	void PODQueueBasePattern<Semaphore>::push()
	{
		m_elements.notify();
	}

	template<class Semaphore>
	void PODQueueBasePattern<Semaphore>::pop()
	{
		m_free.notify();
	}

	template<class T, std::size_t kSize, class Semaphore>
	void PODFixedQueuePattern<T, kSize, Semaphore>::initialise()
	{
		PODQueueBasePattern<Semaphore>::initialise(kSize);
		m_start = m_end = 0;
	}

	template<class T, std::size_t kSize, class Semaphore>
	template<class V>
	CR_IMPL(PODFixedQueuePattern<T, kSize, Semaphore>::PushPattern<V>)
		CR_AWAIT(queue->free());
		util::assign(
			queue->m_values[queue->m_end++],
			std::forward<V>(value));
		if(queue->m_values.size() == queue->m_end)
			queue->m_end = 0;
		queue->push();
	CR_FINALLY
	CR_IMPL_END

	template<class T, std::size_t kSize, class Semaphore>
	CR_IMPL(PODFixedQueuePattern<T, kSize, Semaphore>::Pop)
		CR_AWAIT(queue->elements());
		util::assign(*target, std::move(queue->m_values[queue->m_start++]));
		if(queue->m_values.size() == queue->m_start)
			queue->m_start = 0;
		queue->pop();
	CR_FINALLY
	CR_IMPL_END

	template<std::size_t kSize, class Semaphore>
	void PODFixedQueuePattern<void, kSize, Semaphore>::initialise()
	{
		PODQueueBasePattern<Semaphore>::initialise(kSize);
	}

	template<std::size_t kSize, class Semaphore>
	CR_IMPL(PODFixedQueuePattern<void, kSize, Semaphore>::Push)
		CR_AWAIT(queue->free());
		queue->push();
	CR_FINALLY
	CR_IMPL_END

	template<std::size_t kSize, class Semaphore>
	CR_IMPL(PODFixedQueuePattern<void, kSize, Semaphore>::Pop)
		CR_AWAIT(queue->elements());
		queue->pop();
	CR_FINALLY
	CR_IMPL_END

	template<class T, std::size_t kSize, class Semaphore>
	FixedQueuePattern<T, kSize, Semaphore>::FixedQueuePattern()
	{
		initialise();
	}
}