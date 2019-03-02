namespace cr::detail
{
	template<class DerivedCoroutine>
	bool PlainCoroutineHelper<DerivedCoroutine>::operator()()
	{
		assert(libcr_magic_number == LIBCR_MAGIC_NUMBER);

		return static_cast<DerivedCoroutine *>(this)->_cr_implementation();
	}
}