namespace cr
{
	template<class ConditionVariable>
	SchedulerBase<ConditionVariable> SchedulerBase<ConditionVariable>::s_instance;

	template<class ConditionVariable>
	SchedulerBase<ConditionVariable> &SchedulerBase<ConditionVariable>::instance()
	{
		return s_instance;
	}

	template<class ConditionVariable>
	void SchedulerBase<ConditionVariable>::initialise(
		std::size_t)
	{
	}

	template<class ConditionVariable>
	bool SchedulerBase<ConditionVariable>::schedule(
		std::size_t)
	{
		return m_cv.notify_all();
	}

	template<class ConditionVariable>
	constexpr typename ConditionVariable::WaitCall SchedulerBase<ConditionVariable>::enqueue()
	{
		return m_cv.wait();
	}
}