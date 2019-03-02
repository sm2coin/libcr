namespace cr::detail
{
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
}