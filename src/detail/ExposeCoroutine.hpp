/** @file ExposeCoroutine.hpp
	Contains the ExposeCoroutine helper class. */
#ifndef __libcr_detail_exposecoroutine_hpp_defined
#define __libcr_detail_exposecoroutine_hpp_defined

#ifndef __libcr_coroutine_hpp_defined
#error libcr/Coroutine.hpp must be included before libcr/detail/ExposeCoroutine.hpp
#endif

#include "CoroutineHelper.hpp"

namespace cr::detail
{
	/** Helper class that exposes the coroutine base to other coroutines. */
	class ExposeCoroutine
	{
		template<class T>
		friend class CoroutineHelper;
	protected:
		template<
			class T,
			class = typename std::enable_if<
				std::is_base_of<Coroutine, T>::value
			>::type>
		/** Returns the nested coroutine base pointer of a nested coroutine.
		@param[in] coroutine:
			The coroutine whose base to expose.
		@return
			The coroutine's base. */
		static constexpr Coroutine * base(
			T * coroutine);

		template<
			class T,
			class = typename std::enable_if<
				std::is_base_of<Coroutine, T>::value
			>::type>
		/** Directly calls a coroutine (link time address). */
		static inline void invoke(
			T &coroutine);
	};
}

#include "ExposeCoroutine.inl"

#endif