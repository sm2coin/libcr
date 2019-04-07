/** @file Thread.hpp
	Contains the thread identifier type used to implement thread affinity. */
#ifndef __libcr_detail_thread_hpp_defined
#define __libcr_detail_thread_hpp_defined

#include <cinttypes>

namespace cr::detail
{
	/** Thread identifier type used to implement thread affinity. */
	enum class Thread : std::uint16_t
	{
		/** Invalid thread identifier. */
		kInvalid = (std::uint16_t)~0
	};

	/** The maximum number of threads supported. */
	constexpr std::size_t max_threads();

	/** Checks whether a thread identifier is valid. */
	constexpr bool valid(
		Thread id);
}

#include "Thread.inl"

#endif