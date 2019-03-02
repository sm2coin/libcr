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

	template class PODMutexBase<PODConditionVariable>;
	template class PODMutexBase<PODFIFOConditionVariable>;
	template class MutexBase<PODConditionVariable>;
	template class MutexBase<PODFIFOConditionVariable>;
}