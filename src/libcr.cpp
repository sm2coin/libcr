#include "libcr.hpp"
#include <iostream>

namespace cr
{
	void CoroutineBase::prepare(
		impl_t coroutine_start)
	{
#ifdef LIBCR_DEBUG
		m_magic_number = LIBCR_MAGIC_NUMBER;
#endif
		m_coroutine_start = coroutine_start;
		m_coroutine_ip = 0;
	}
}