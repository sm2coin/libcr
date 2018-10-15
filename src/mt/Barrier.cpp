#include "Barrier.hpp"

namespace cr::mt
{
	sync::mayblock Barrier::WaitCall::libcr_wait(
		Coroutine * coroutine)
	{
		std::lock_guard lock(m_barrier.m_mutex);
		return m_barrier.wait().libcr_wait(coroutine);
	}
}