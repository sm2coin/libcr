#ifndef __libcr_specific_hpp_defined
#define __libcr_specific_hpp_defined

#include "helpermacros.hpp"

/** @def CR_RESTORE
	Returns to the last saved execution state. */
#ifdef __GNUC__

#define LIBCR_HELPER_SAVE(id) do { \
	m_coroutine_ip = &&LIBCR_HELPER_LABEL(id); \
} while(0)

#define CR_RESTORE do { \
	goto *m_coroutine_ip; \
} while(0)

#else
#error "Compiler not supported!"
#endif

#endif