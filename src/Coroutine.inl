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
}