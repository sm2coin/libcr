namespace cr::detail
{
	Coroutine * AtomicNextPointer::update(
		Coroutine * next)
	{
		return exchange(next, std::memory_order_relaxed);
	}

	Coroutine * AtomicNextPointer::wait_weak()
	{
		Coroutine * next;
		while(!(next = load_weak(std::memory_order_relaxed)));
		return next;
	}

	Coroutine * AtomicNextPointer::wait_strong()
	{
		Coroutine * next = load_strong(std::memory_order_relaxed);
		while(!next)
			next = load_weak(std::memory_order_relaxed);
		return next;
	}

	void AtomicNextPointer::release()
	{
		store(nullptr, std::memory_order_release);
	}

	void AtomicNextPointer::release_with_next(
		Coroutine * next)
	{
		store(next, std::memory_order_release);
	}

	Coroutine * AtomicNextPointer::acquire_strong()
	{
		return load_strong(std::memory_order_acquire);
	}

	Coroutine * AtomicNextPointer::acquire_weak()
	{
		return load_weak(std::memory_order_acquire);
	}
}