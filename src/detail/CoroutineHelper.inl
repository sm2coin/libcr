namespace cr::detail
{
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
	template<class ...Args>
	void CoroutineHelper<DerivedCoroutine>::prepare(
		nullptr_t,
		Args&& ...args)
	{
		Coroutine::prepare(
			static_cast<impl_t>(&DerivedCoroutine::_cr_implementation),
			(Context *)nullptr);

		static_cast<DerivedCoroutine *>(this)->cr_prepare(
			std::forward<Args>(args)...);
	}


	template<class DerivedCoroutine>
	template<class ...Args>
	void CoroutineHelper<DerivedCoroutine>::start(
		Context * context,
		Args&& ...args)
	{
		prepare(context, std::forward<Args>(args)...);
		start_prepared();
	}

	template<class DerivedCoroutine>
	template<class ...Args>
	void CoroutineHelper<DerivedCoroutine>::start(
		Coroutine * parent,
		Args&& ...args)
	{
		prepare(parent, std::forward<Args>(args)...);
		start_prepared();
	}

	template<class DerivedCoroutine>
	template<class ...Args>
	void CoroutineHelper<DerivedCoroutine>::start(
		nullptr_t,
		Args&& ...args)
	{
		prepare(nullptr, std::forward<Args>(args)...);
		start_prepared();
	}

	template<class DerivedCoroutine>
	void CoroutineHelper<DerivedCoroutine>::start_prepared()
	{
		assert(libcr_magic_number == LIBCR_MAGIC_NUMBER);

		static_cast<DerivedCoroutine *>(this)->_cr_implementation();
	}
}