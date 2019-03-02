#include "Protothread.hpp"

namespace cr
{
	void Protothread::prepare()
	{
#ifdef LIBCR_DEBUG
		libcr_magic_number = LIBCR_MAGIC_NUMBER;
#endif
		libcr_instruction_pointer = nullptr;
	}
}