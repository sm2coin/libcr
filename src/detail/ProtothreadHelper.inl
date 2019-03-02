namespace cr::detail
{
	template<class DerivedProtothread>
	bool ProtothreadHelper<DerivedProtothread>::operator()()
	{
		assert(static_cast<DerivedProtothread&>(*this).libcr_magic_number == LIBCR_MAGIC_NUMBER);
		return static_cast<DerivedProtothread&>(*this)._cr_implementation();
	}

	template<class DerivedProtothread>
	template<class ...Args>
	void ProtothreadHelper<DerivedProtothread>::prepare(
		Args&& ...args)
	{
		Protothread::prepare();

		static_cast<DerivedProtothread&>(*this).cr_prepare(
			std::forward<Args>(args)...);
	}
}