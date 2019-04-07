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
		// Leave libcr_next_waiting uninitialised!
		libcr_thread = detail::Thread::kInvalid;
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
		// Leave libcr_next_waiting uninitialised!
		libcr_thread = detail::Thread::kInvalid;
	}
}