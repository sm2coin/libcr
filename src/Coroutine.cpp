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
		impl_t coroutine,
		Context * context)
	{
		PlainCoroutine::prepare();
		libcr_parent = nullptr;
		libcr_context = context;
		libcr_coroutine = coroutine;
		std::atomic_init(&libcr_next_waiting, (Coroutine *)nullptr);
	}

	void Coroutine::prepare(
		impl_t coroutine,
		Coroutine * parent)
	{
		PlainCoroutine::prepare();
		libcr_parent = parent;
		libcr_context = parent->libcr_context;
		libcr_coroutine = coroutine;
		std::atomic_init(&libcr_next_waiting, (Coroutine *)nullptr);
	}

	void Coroutine::pause()
	{
		assert(!waiting());
		libcr_next_waiting.store(this, std::memory_order_release);
	}

	void Coroutine::resume()
	{
		assert(waiting());
		libcr_next_waiting.exchange(nullptr, std::memory_order_acquire);
	}
}