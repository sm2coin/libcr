/** @file Protothread.hpp
	Contains the protothread class. */
#ifndef __libcr_protothread_hpp_defined
#define __libcr_protothread_hpp_defined

#include "helpermacros.hpp"
#include <cinttypes>


#ifdef LIBCR_DEBUG
/** @def LIBCR_MAGIC_NUMBER
	A random number that is used to check whether a coroutine was properly initialised. */
#define LIBCR_MAGIC_NUMBER static_cast<std::size_t>(0x38eee375b1b29314)
#endif


namespace cr
{
#ifdef LIBCR_COMPACT_IP
	typedef std::uint16_t ip_t;
#else
	/** Instruction pointer type.
		This is used to save the execution of a protothread. */
	typedef void *ip_t;
#endif

	/** Basic protothread state. */
	class Protothread
	{
	public:
#ifdef LIBCR_DEBUG
		/** Used when debugging, to check whether the protothread was initialised. */
		std::size_t libcr_magic_number;
#endif
		/** Contains the last saved instruction pointer. */
		ip_t libcr_instruction_pointer;

		/** Prepares the protothread for execution. */
		void prepare();
	};
}

#endif