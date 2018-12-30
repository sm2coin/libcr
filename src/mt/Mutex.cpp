#include "Mutex.hpp"
#include "../Coroutine.hpp"
#include "Promise.hpp"

namespace cr::mt
{
	template<class ConditionVariable>
	void PODMutexBase::initialise()
	{
		PODConsumableEventBase<ConditionVariable>::initialise(true);
	}

	template<class ConditionVariable>
	MutexBase<ConditionVariable>::Mutex()
	{
		PODMutexBase<ConditionVariable>::initialise();
	}

	template class PODMutex<PODConditionVariable>;
	template class PODMutex<PODFIFOConditionVariable>;
	template class Mutex<PODConditionVariable>;
	template class Mutex<PODFIFOConditionVariable>;
}