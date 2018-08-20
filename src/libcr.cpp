#include "libcr.hpp"

namespace cr
{
	void Coroutine::prepare()
	{
		m_coroutine_line = LIBCR_MAGIC_NUMBER;
	}
}