/** @file CoroutineHelper.hpp
	Contains the CoroutineHelper helper class. */
#ifndef __libcr_detail_coroutinehelper_hpp_defined
#define __libcr_detail_coroutinehelper_hpp_defined

#ifndef __libcr_coroutine_hpp_defined
#error libcr/Coroutine.hpp must be included before libcr/detail/CoroutineHelper.hpp
#endif

#include "../Context.hpp"

namespace cr::detail
{
	template<class DerivedCoroutine>
	/** Helper type for initialising and calling coroutines.
	@tparam DerivedCoroutine:
		The deriving type. */
	class CoroutineHelper : public Coroutine
	{
		friend DerivedCoroutine;
	public:
		template<class ...Args>
		/** Prepares the coroutine as the root coroutine. */
		inline void prepare(
			Context * context,
			Args&& ...args);

		template<class ...Args>
		/** Prepares the coroutine as a child coroutine.
		@param[in] parent:
			The parent coroutine. */
		inline void prepare(
			Coroutine * parent,
			Args&& ...args);

		template<class ...Args>
		/** Prepares the coroutine as the root coroutine, without a context. */
		inline void prepare(
			nullptr_t,
			Args&& ...args);


		template<class ...Args>
		/** Prepares the coroutine as the root coroutine, and starts it. */
		inline void start(
			Context * context,
			Args&& ...args);

		template<class ...Args>
		/** Prepares the coroutine as a child coroutine, and starts it.
		@param[in] parent:
			The parent coroutine. */
		inline void start(
			Coroutine * parent,
			Args&& ...args);

		template<class ...Args>
		/** Prepares the coroutine as the root coroutine, without a context, and starts it. */
		inline void start(
			nullptr_t,
			Args&& ...args);

		/** Starts the coroutine after `prepare()` has been called.
			This should only be called once per coroutine! */
		inline void start_prepared();
	};
}

#include "CoroutineHelper.inl"

#endif