/** @file Context.hpp
	Contains the task-local storage context type. */
#ifndef __libcr_context_hpp_defined
#define __libcr_context_hpp_defined

#include "helpermacros.hpp"

namespace cr
{
	/** Base class for coroutine execution contexts.
		This type acts like a coroutine version of `thread_local` storage. To use this, simply create a custom context type that derives from this class. The deriving class should also derive from multiple smaller, isolated classes that make up module-specific or function-specific parts of the context. */
	class Context
	{
	public:
		template<class T>
		/** Retrieves a part of the context.
		@tparam[in] T:
			The part of the context to retrieve.
		@return
			The requested context.
		@throws std::bad_cast
			If T is not part of the context. */
		inline T &local();

		// Make Context abstract.
		virtual ~Context() = 0;
	};
}

#include "Context.inl"

#endif