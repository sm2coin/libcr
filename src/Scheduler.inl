namespace cr
{
	template<class ConditionVariable>
	SchedulerPattern<ConditionVariable> SchedulerPattern<ConditionVariable>::s_instance;

	template<class ConditionVariable>
	SchedulerPattern<ConditionVariable> &SchedulerPattern<ConditionVariable>::instance()
	{
		return s_instance;
	}

	template<class ConditionVariable>
	void SchedulerPattern<ConditionVariable>::initialise(
		std::size_t)
	{
	}

	template<class ConditionVariable>
	bool SchedulerPattern<ConditionVariable>::schedule(
		std::size_t)
	{
		return m_cv.notify_all();
	}

	template<class ConditionVariable>
	constexpr typename ConditionVariable::WaitCall SchedulerPattern<ConditionVariable>::enqueue()
	{
		return m_cv.wait();
	}
}