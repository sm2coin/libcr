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
}