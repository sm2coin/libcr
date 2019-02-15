#include <cinttypes>
namespace cr
{
	template<class DerivedCoroutine>
	bool PlainCoroutineHelper<DerivedCoroutine>::operator()()
	{
		assert(libcr_magic_number == LIBCR_MAGIC_NUMBER);

		return static_cast<DerivedCoroutine *>(this)->_cr_implementation();
	}

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

	template<class T, class>
	constexpr Coroutine * ExposeCoroutine::base(
		T * coroutine)
	{
		return coroutine;
	}

	template<class T, class>
	void ExposeCoroutine::invoke(
		T & coroutine)
	{
		assert(coroutine.libcr_magic_number == LIBCR_MAGIC_NUMBER);
		coroutine._cr_implementation();
	}

	template<class DerivedCoroutine>
	template<class ...Args>
	void CoroutineHelper<DerivedCoroutine>::prepare(
		Context * context,
		Args&& ...args)
	{
		Coroutine::prepare(
			static_cast<impl_t>(&DerivedCoroutine::_cr_implementation),
			context);

		static_cast<DerivedCoroutine *>(this)->cr_prepare(
			std::forward<Args>(args)...);
	}

	template<class DerivedCoroutine>
	template<class ...Args>
	void CoroutineHelper<DerivedCoroutine>::prepare(
		Coroutine * parent,
		Args&& ...args)
	{
		Coroutine::prepare(
			static_cast<impl_t>(&DerivedCoroutine::_cr_implementation),
			parent);

		static_cast<DerivedCoroutine *>(this)->cr_prepare(
			std::forward<Args>(args)...);
	}

	template<class DerivedCoroutine>
	void CoroutineHelper<DerivedCoroutine>::start()
	{
		assert(libcr_magic_number == LIBCR_MAGIC_NUMBER);

		static_cast<DerivedCoroutine *>(this)->_cr_implementation();
	}
}