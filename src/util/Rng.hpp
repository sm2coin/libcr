#ifndef __libcr_util_rng_hpp_defined
#define __libcr_util_rng_hpp_defined

#include <cinttypes>

namespace cr::util
{
	class Rng
	{
		std::uint16_t m_index;
	public:
		Rng(std::uint16_t seed):
			m_index(seed ^ 0x69d0) {}
		// Get a pseudo-random byte.
		inline std::uint8_t byte();
		// Flip a coin to be true in roughly odds/255 of cases.
		inline bool flip(std::uint8_t odds);
	};
}

#include "Rng.inl"

#endif