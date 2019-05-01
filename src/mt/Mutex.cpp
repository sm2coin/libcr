#include "Mutex.hpp"
#include "../Coroutine.hpp"
#include "Promise.hpp"

namespace cr::mt
{
	template<class ConditionVariable>
	void PODMutexPattern<ConditionVariable>::initialise()
	{
		PODConsumableEventPattern<ConditionVariable>::initialise(true);
	}

	template class PODMutexPattern<PODConditionVariable>;
	template class PODMutexPattern<PODFIFOConditionVariable>;
	template class MutexPattern<PODConditionVariable>;
	template class MutexPattern<PODFIFOConditionVariable>;
}