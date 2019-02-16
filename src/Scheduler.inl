namespace cr
{
	template<class ConditionVariable>
	SchedulerBase<ConditionVariable> &SchedulerBase<ConditionVariable>::instance()
	{
		static SchedulerBase<ConditionVariable> instance;
		return instance;
	}

	template<class ConditionVariable>
	bool SchedulerBase<ConditionVariable>::schedule()
	{
		return m_cv.notify_all();
	}

	template<class ConditionVariable>
	constexpr typename ConditionVariable::WaitCall SchedulerBase<ConditionVariable>::enqueue()
	{
		return m_cv.wait();
	}
}