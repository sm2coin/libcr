namespace cr::detail
{
	constexpr std::size_t max_threads()
	{
		 return (std::size_t)Thread::kInvalid;
	}

	constexpr bool valid(
		Thread id)
	{
		return id != Thread::kInvalid;
	}
}