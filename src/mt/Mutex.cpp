#include "Mutex.hpp"
#include "../Coroutine.hpp"
#include "Promise.hpp"

namespace cr::mt
{
	template<class ConditionVariable>
	void PODMutexBase<ConditionVariable>::initialise()
	{
		PODConsumableEventBase<ConditionVariable>::initialise(true);
	}

	template<class ConditionVariable>
	bool PODMutexBase<ConditionVariable>::try_lock()
	{
		return PODConsumableEventBase<ConditionVariable>::try_consume();
	}

	template<class ConditionVariable>
	MutexBase<ConditionVariable>::MutexBase()
	{
		PODMutexBase<ConditionVariable>::initialise();
	}

	template class PODMutexBase<PODConditionVariable>;
	template class PODMutexBase<PODFIFOConditionVariable>;
	template class MutexBase<PODConditionVariable>;
	template class MutexBase<PODFIFOConditionVariable>;
}