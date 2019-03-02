/** @file ProtothreadHelper.hpp
	Contains the protothread helper class. */
#ifndef __libcr_detail_protothreadhelper_hpp_defined
#define __libcr_detail_protothreadhelper_hpp_defined

#include "../Protothread.hpp"

namespace cr::detail
{
	template<class DerivedProtothread>
	/** Helper class for easier initialisation of protothreads.
		Protothreads are not optimised for nesting, entering into a callstack has linear complexity. However, protothreads have less memory overhead and are generally faster.
	@tparam DerivedProtothread:
		The protothread type that derives from this type. */
	class ProtothreadHelper : protected cr::Protothread
	{
	public:
		/** Executes the coroutine.
		@return
			Whether the coroutine is done. */
		inline bool operator()();

		template<class ...Args>
		/** Prepares the protothread.
		@param[in] args:
			The arguments to be passed to the protothread's initialisation function. */
		inline void prepare(
			Args&& ...args);
	};
}

#include "ProtothreadHelper.inl"

#endif