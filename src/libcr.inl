#include <cinttypes>
namespace cr
{
	template<class T, class ...Args>
	void prepare(
		T &self,
		Args &&... args)
	{
		self.prepare(std::forward<Args>(args)...);
	}

	bool CoroutineBase::operator()()
	{
		assert(m_magic_number == LIBCR_MAGIC_NUMBER);

		typedef bool (*lambda_t)(void *);

		return reinterpret_cast<lambda_t&>(m_coroutine_start)(this);
	}

	template<class DerivedCoroutine>
	void Coroutine<DerivedCoroutine>::prepare()
	{
		CoroutineBase::prepare([](void * self)->bool
			{
				return static_cast<DerivedCoroutine *>(self)->_cr_implementation();
			});
	}

	template<class DerivedCoroutine>
	bool Coroutine<DerivedCoroutine>::operator()()
	{
		assert(
			static_cast<Coroutine<DerivedCoroutine> *>(
				static_cast<DerivedCoroutine *>(
					reinterpret_cast<void*>(this))) == this
			&& "DerivedCoroutine `this` must be == Coroutine `this`.");

		return CoroutineBase::operator()();
	}

	bool NestCoroutineBase::is_root() const
	{
		return libcr_root == this;
	}

	template<class T, class>
	constexpr NestCoroutineBase * ExposeNestCoroutineBase::base(
		T * coroutine)
	{
		return coroutine;
	}

	template<class DerivedCoroutine>
	void NestCoroutine<DerivedCoroutine>::prepare(
		NestCoroutineBase * root,
		NestCoroutineBase * parent)
	{
		NestCoroutineBase::libcr_root = root;
		NestCoroutineBase::libcr_stack = parent;

		CoroutineBase::prepare([](void * self)->bool
		{
			return static_cast<DerivedCoroutine *>(self)->_cr_implementation();
		});
	}

	template<class DerivedCoroutine>
	void NestCoroutine<DerivedCoroutine>::prepare()
	{
		NestCoroutineBase::libcr_root = this;
		NestCoroutineBase::libcr_stack = this;
		CoroutineBase::prepare(&lambda);
	}

	template<class DerivedCoroutine>
	bool NestCoroutine<DerivedCoroutine>::operator()()
	{
		assert((void*)this == (CoroutineBase*) this);
		return NestCoroutineBase::libcr_root->libcr_stack->CoroutineBase::operator()();
	}

	template<class DerivedCoroutine>
	bool NestCoroutine<DerivedCoroutine>::lambda(
		void * self)
	{
		return static_cast<DerivedCoroutine *>(self)->_cr_implementation();
	}
}