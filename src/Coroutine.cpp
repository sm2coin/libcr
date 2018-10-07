#include "Coroutine.hpp"

namespace cr
{
	void PlainCoroutine::prepare()
	{
#ifdef LIBCR_DEBUG
		libcr_magic_number = LIBCR_MAGIC_NUMBER;
#endif
		libcr_coroutine_ip = nullptr;
	}

	void Coroutine::prepare(
		impl_t coroutine)
	{
		PlainCoroutine::prepare();
		libcr_root = this;
		libcr_stack = this;
		libcr_coroutine = coroutine;
		libcr_next_waiting = nullptr;
	}

	void Coroutine::prepare(
		impl_t coroutine,
		Coroutine * parent)
	{
		PlainCoroutine::prepare();
		libcr_root = parent->libcr_root;
		libcr_stack = parent;
		libcr_coroutine = coroutine;
		libcr_next_waiting = nullptr;
	}
}