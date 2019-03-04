namespace cr::util
{
	template<class Coroutine>
	Coroutine * AutoCoroutine<Coroutine>::operator->()
	{
		return &coroutine;
	}

	template<class Coroutine>
	Coroutine &AutoCoroutine<Coroutine>::operator*()
	{
		return coroutine;
	}

	template<class Coroutine>
	template<class ...Args>
	void AutoCoroutine<Coroutine>::cr_prepare(
		Args&& ...args)
	{
		coroutine.prepare(
			this,
			std::forward<Args>(args)...);
	}

	template<class Coroutine>
	void AutoCoroutine<Coroutine>::cr_destroy()
	{
		delete this;
	}

	template<class Coroutine>
	CR_IMPL(AutoCoroutine<Coroutine>)
		CR_CALL_PREPARED(coroutine);
	CR_IMPL_END

	template<class Coroutine>
	inline AutoCoroutine<Coroutine> &make_auto()
	{
		return *(new AutoCoroutine<Coroutine>);
	}
}