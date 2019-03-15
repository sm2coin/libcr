namespace cr::util
{
	template<class T>
	T Atomic<T>::load_strong(std::memory_order order)
	{
		T expected = T();
		std::atomic<T>::compare_exchange_strong(
			expected,
			expected,
			order,
			order);
		return expected;
	}

	template<class T>
	T Atomic<T>::load_weak(std::memory_order order)
	{
		return std::atomic<T>::load(order);
	}
}