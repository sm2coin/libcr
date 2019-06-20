/** @file specific.hpp
	Contains compiler specific versions of macros that cannot be written in standard C++. */
#ifndef __libcr_specific_hpp_defined
#define __libcr_specific_hpp_defined

#include "helpermacros.hpp"

/** @def CR_RESTORE
	Returns to the last saved execution state. */
#ifdef __GNUC__


#define LIBCR_HELPER_CONCAT(a,b) a##b
#define LIBCR_HELPER_LABEL(id) LIBCR_HELPER_CONCAT(libcr_label_,id)

#ifdef LIBCR_COMPACT_IP
#define LIBCR_HELPER_SAVE(id) do { \
	::cr::Protothread::libcr_instruction_pointer = std::uintptr_t(&&LIBCR_HELPER_LABEL(id)) - std::uintptr_t(&&LIBCR_HELPER_LABEL(start)); \
} while(0)
#else
#define LIBCR_HELPER_SAVE(id) do { \
	::cr::Protothread::libcr_instruction_pointer = &&LIBCR_HELPER_LABEL(id); \
} while(0)
#endif

#ifdef LIBCR_COMPACT_IP
#define CR_RESTORE do { \
	goto *(void *)((std::uintptr_t)std::uintptr_t(&&LIBCR_HELPER_LABEL(start)) + ::cr::Protothread::libcr_instruction_pointer); \
} while(0)
#else
#define CR_RESTORE do { \
	goto *::cr::Protothread::libcr_instruction_pointer; \
} while(0)
#endif

#else
#error "Compiler not supported!"
#endif

#endif