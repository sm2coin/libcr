namespace cr::sync
{

	template<class ConditionVariable>
	void PODFixedQueueBasePattern<ConditionVariable>::initialise(
		std::size_t capacity)
	{
		m_elements.initialise(0);
		m_free.initialise(capacity);
	}

	template<class ConditionVariable>
	constexpr typename PODFixedQueueBasePattern<ConditionVariable>::Semaphore::WaitCall PODFixedQueueBasePattern<ConditionVariable>::free()
	{
		return m_free.wait();
	}

	template<class ConditionVariable>
	constexpr typename PODFixedQueueBasePattern<ConditionVariable>::Semaphore::WaitCall PODFixedQueueBasePattern<ConditionVariable>::elements()
	{
		return m_elements.wait();
	}

	template<class ConditionVariable>
	void PODFixedQueueBasePattern<ConditionVariable>::push()
	{
		m_elements.notify();
	}

	template<class ConditionVariable>
	void PODFixedQueueBasePattern<ConditionVariable>::pop()
	{
		m_free.notify();
	}

	template<class T, std::size_t kSize, class ConditionVariable>
	void PODFixedQueuePattern<T, kSize, ConditionVariable>::initialise()
	{
		PODFixedQueueBasePattern<ConditionVariable>::initialise(kSize);
		m_start = m_end = 0;
	}

	template<class T, std::size_t kSize, class ConditionVariable>
	template<class V>
	CR_IMPL(PODFixedQueuePattern<T, kSize, ConditionVariable>::PushPattern<V>)
		CR_AWAIT(queue->free());
		util::assign(queue->m_values[queue->m_end++], value);
		if(queue->m_values.size() == queue->m_end)
			queue->m_end = 0;
		queue->push();
	CR_IMPL_END

	template<class T, std::size_t kSize, class ConditionVariable>
	CR_IMPL(PODFixedQueuePattern<T, kSize, ConditionVariable>::Pop)
		CR_AWAIT(queue->elements());
		util::assign(*target, std::move(queue->m_values[queue->m_start++]));
		if(queue->m_values.size() == queue->m_start)
			queue->m_start = 0;
		queue->pop();
	CR_IMPL_END

	template<std::size_t kSize, class ConditionVariable>
	void PODFixedQueuePattern<void, kSize, ConditionVariable>::initialise()
	{
		PODFixedQueueBasePattern<ConditionVariable>::initialise(kSize);
	}

	template<std::size_t kSize, class ConditionVariable>
	CR_IMPL(PODFixedQueuePattern<void, kSize, ConditionVariable>::Push)
		CR_AWAIT(queue->free());
		queue->push();
	CR_IMPL_END

	template<std::size_t kSize, class ConditionVariable>
	CR_IMPL(PODFixedQueuePattern<void, kSize, ConditionVariable>::Pop)
		CR_AWAIT(queue->elements());
		queue->pop();
	CR_IMPL_END

	template<class T, std::size_t kSize, class ConditionVariable>
	FixedQueuePattern<T, kSize, ConditionVariable>::FixedQueuePattern()
	{
		initialise();
	}
}