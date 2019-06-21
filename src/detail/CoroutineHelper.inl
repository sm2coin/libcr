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

	template<class DerivedCoroutine>
	constexpr CoroutineHelper<DerivedCoroutine>::LibcrChildStartCall::LibcrChildStartCall(
		Coroutine * parent,
		DerivedCoroutine * child):
		m_parent(parent),
		m_child(child)
	{
	}

	template<class DerivedCoroutine>
	template<class ...Args>
	void CoroutineHelper<DerivedCoroutine>::LibcrChildStartCall::operator()(
		Args&& ...args) const
	{
		m_child->start(
			m_parent,
			std::forward<Args>(args)...);
	}

	template<class DerivedCoroutine>
	constexpr typename CoroutineHelper<DerivedCoroutine>::LibcrChildStartCall
		CoroutineHelper<DerivedCoroutine>::libcr_child_start_call(
			Coroutine * parent)
	{
		return LibcrChildStartCall(parent, static_cast<DerivedCoroutine *>(this));
	}
}