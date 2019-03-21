/** @file RefConv.hpp
	Contains helper types that convert reference types to appropriate types when passed as arguments to coroutines. */
#ifndef __libcr_util_reftoptr_hpp_defined
#define __libcr_util_reftoptr_hpp_defined

#include <utility>

namespace cr::util
{
	template<class T>
	struct RefConv
	{
		typedef T type;
	};

	template<class T>
	struct RefConv<T &>
	{
		typedef T * type;
	};

	template<class T>
	struct RefConv<T const&>
	{
		typedef T type;
	};

	template<class T>
	struct RefConv<T &&>
	{
		typedef T type;
	};

	template<class T>
	struct RefConv<T const&&>
	{
		typedef T type;
	};

	template<class T>
	using ref_conv_t = typename RefConv<T>::type;

	template<class T>
	inline T &assign(T &a, T const& b)
	{
		return a = b;
	}

	template<class T>
	inline T &assign(T *&a, T &b)
	{
		return a = &b;
	}

	template<class T>
	inline T &assign(T &a, T &&b)
	{
		return a = std::move(b);
	}

	template<class T>
	inline T &assign(T &a, T const&& b)
	{
		return a = std::move(b);
	}
}

#endif