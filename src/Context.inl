namespace cr
{
	template<class T>
	inline T &Context::local()
	{
		return dynamic_cast<T&>(*this);
	}
}