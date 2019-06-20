#include "Protothread.hpp"

namespace cr
{
	void Protothread::prepare()
	{
#ifdef LIBCR_DEBUG
		libcr_magic_number = LIBCR_MAGIC_NUMBER;
#endif
#ifdef LIBCR_COMPACT_IP
		libcr_instruction_pointer = 0;
#else
		libcr_instruction_pointer = nullptr;
#endif
	}
}