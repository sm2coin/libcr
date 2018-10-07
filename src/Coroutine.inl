#include <cinttypes>
namespace cr
{
	template<class DerivedCoroutine>
	bool PlainCoroutineHelper<DerivedCoroutine>::operator()()
	{
		assert(libcr_magic_number == LIBCR_MAGIC_NUMBER);

		return static_cast<DerivedCoroutine *>(this)->_cr_implementation();
	}

	bool Coroutine::is_root() const
	{
		return libcr_root == this;
	}

	bool Coroutine::is_child() const
	{
		return libcr_root->libcr_stack == this;
	}

	bool Coroutine::operator()()
	{
		assert(libcr_magic_number == LIBCR_MAGIC_NUMBER);
		assert(is_root() && "Only call the root coroutine.");

		return (libcr_stack->*libcr_stack->libcr_coroutine)();
	}

	bool Coroutine::directly_call_child()
	{
		assert(libcr_magic_number == LIBCR_MAGIC_NUMBER);
		assert(is_child() && "Only call the child coroutine.");

		return (this->*libcr_coroutine)();
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

	template<class T, class>
	constexpr Coroutine * ExposeCoroutine::base(
		T * coroutine)
	{
		return coroutine;
	}

	template<class T, class>
	bool ExposeCoroutine::directly_call_child(
		T & coroutine)
	{
		assert(coroutine.libcr_magic_number == LIBCR_MAGIC_NUMBER);
		assert(coroutine.is_child() && "Only call the child coroutine.");

		return coroutine._cr_implementation();
	}

	template<class DerivedCoroutine>
	void CoroutineHelper<DerivedCoroutine>::prepare()
	{
		Coroutine::prepare(
			static_cast<impl_t>(&DerivedCoroutine::_cr_implementation));
	}

	template<class DerivedCoroutine>
	void CoroutineHelper<DerivedCoroutine>::prepare(
		Coroutine * parent)
	{
		Coroutine::prepare(
			static_cast<impl_t>(&DerivedCoroutine::_cr_implementation),
			parent);
	}
}