#include "Coroutine.hpp"

namespace cr
{
	void Coroutine::prepare(
		impl_t coroutine,
		Context * context)
	{
		Protothread::prepare();
		libcr_parent = nullptr;
		libcr_context = context;
		libcr_coroutine = coroutine;
		libcr_error = false;
		std::atomic_init(&libcr_next_waiting, (Coroutine *)nullptr);
	}

	void Coroutine::prepare(
		impl_t coroutine,
		Coroutine * parent)
	{
		Protothread::prepare();
		libcr_parent = parent;
		libcr_context = parent->libcr_context;
		libcr_coroutine = coroutine;
		libcr_error = false;
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