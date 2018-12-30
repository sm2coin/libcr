namespace cr::mt
{
	template<class T, class Event>
	bool PODFutureBase<T, Event>::fulfill(
		T && value)
	{
		// Is the promise already fulfilled?
		if(fulfilled())
			return false;

		detail::LockGuard lock;
		// Try to lock the future.
		// If it was already locked, then we were too late.
		if(lock.try_lock(m_mutex))
		{
			// Was it locked in the mean time?
			if(fulfilled())
				return false;

			// Set the future's value and fulfill it.
			m_value = std::move(value);
			Event::fire();
		}

		// Whether we were the ones to fulfill the promise.
		return lock.locked();
	}

	template<class T, class Event>
	bool PODFutureBase<T, Event>::fulfill(
		T const& value)
	{
		// Is the promise already fulfilled?
		if(Event::active())
			return false;

		detail::LockGuard lock;
		// Try to lock the future.
		// If it was already locked, then we were too late.
		if(lock.try_lock(m_mutex))
		{
			// Was it locked in the mean time?
			if(Event::active())
				return false;

			// Set the future's value and fulfill it.
			m_value = value;
			Event::fire();
		}

		// Whether we were the ones to fulfill the promise.
		return lock.locked();
	}

	template<class T, class Event>
	void PODFutureBase<T, Event>::initialise()
	{
		Event::initialise();
		m_mutex.initialise();
	}

	template<class T, class Event>
	util::copy_t<T> PODFutureBase<T, Event>::value() const
	{
		// The promise must be fulfilled before acquiring its value.
		assert(fulfilled());

		return m_value;
	}
}