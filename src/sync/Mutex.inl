namespace cr::sync
{
	bool PODMutex::is_owner(
		Coroutine * self)
	{
		return self == m_owner;
	}
}