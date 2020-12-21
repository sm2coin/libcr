namespace cr::util
{
	inline std::uint8_t Rng::byte()
	{
		static std::uint8_t const k_nibble[16] alignas(16) = {
			219, 123, 109, 87,
			67, 4, 10, 163,
			134, 137, 218, 206,
			203, 232, 91, 64
		};

		auto byte = k_nibble[m_index & 0xf]
			//^ k_nibble[(m_index >> 4) & 0xf]
			^ k_nibble[(m_index >> 8) & 0xf]
			^ (m_index >> 8) ^ (m_index>>4 & 0xf);
		m_index ^= byte << 8;
		m_index++;
		return byte;
	}

	bool Rng::flip(std::uint8_t odds) {
		return odds & (byte() <= odds);
	}
}