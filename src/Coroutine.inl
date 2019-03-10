#include <cinttypes>
namespace cr
{
	void Coroutine::operator()()
	{
		assert(libcr_magic_number == LIBCR_MAGIC_NUMBER);

		(this->*libcr_coroutine)();
	}

	template<class T>
	T &Coroutine::local()
	{
		return libcr_context->local<T>();
	}

	template<class T>
	bool Coroutine::libcr_unpack_wait(
		T * value)
	{
		return value->libcr_wait(this);
	}

	template<class T>
	bool Coroutine::libcr_unpack_wait(
		T & value)
	{
		return value.libcr_wait(this);
	}

	template<class T>
	bool Coroutine::libcr_unpack_wait(
		T && value)
	{
		return value.libcr_wait(this);
	}

	bool Coroutine::libcr_unpack_wait(
		bool value)
	{
		return value;
	}

	bool Coroutine::waiting() const
	{
		return libcr_next_waiting.load(std::memory_order_relaxed);
	}

	Coroutine * Coroutine::next_waiting() const
	{
		Coroutine * next = libcr_next_waiting.load(std::memory_order_relaxed);
		if(next != this)
			return next;
		else
			return nullptr;
	}

	void Coroutine::release()
	{
		libcr_next_waiting.store(this, std::memory_order_release);
	}

	void Coroutine::set_next_waiting(
		Coroutine * coroutine)
	{
		// Use RMW to keep the release sequence intact.
		[[maybe_unused]] Coroutine * waiting = libcr_next_waiting.exchange(
			coroutine,
			std::memory_order_relaxed);
		assert(!waiting || waiting == this);
	}

	Coroutine * Coroutine::acquire()
	{
		Coroutine * next = libcr_next_waiting.exchange(nullptr, std::memory_order_acquire);
		assert(next != nullptr && "acquired unreleased coroutine!");
		if(next != this)
			return next;
		else
			return nullptr;
	}
}