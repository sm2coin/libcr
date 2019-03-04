/** @file AutoCoroutine.hpp
	Contains the AutoCoroutine helper for dynamic coroutine deletion. */
#ifndef __libcr_util_autocoroutine_hpp_defined
#define __libcr_util_autocoroutine_hpp_defined

#include "../primitives.hpp"

namespace cr::util
{
	template<class Coroutine>
	/** Wrapper for dynamically allocated coroutines, deletes them after they return. */
	TEMPLATE_COROUTINE(AutoCoroutine, (Coroutine), void)
		/** Allows access to the inner coroutine.
			Note that after the coroutine returns, it is deleted. */
		inline Coroutine * operator->();
		/** Allows access to the inner coroutine.
			Note that after the coroutine returns, it is deleted. */
		inline Coroutine &operator*();
	CR_STATE
		Coroutine coroutine;

		template<class ...Args>
		/** Prepares the inner coroutine. */
		inline void cr_prepare(
			Args&& ...args);
		/** Deletes both coroutines. */
		inline void cr_destroy();
	CR_EXTERNAL


	template<class Coroutine>
	/** Creates a self-deleting coroutine. */
	inline AutoCoroutine<Coroutine> &make_auto();
}

#include "AutoCoroutine.inl"

#endif