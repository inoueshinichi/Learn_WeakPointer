#pragma once

#include <assert.h>
#include <type_traits>

namespace is
{
  class PtrInfo;
  
  template <class T>
  class WeakPtrController;
  
  /*弱参照ポインタクラス*/
  template <typename T>
  class WeakPtr
  {
	  friend class WeakPtrController<T>;
	  PtrInfo* m_pPtrInfo{ nullptr };

  public:
	  ~WeakPtr() { PtrInfo::decrement(m_pPtrInfo); }
	  WeakPtr() {}

	  explicit WeakPtr(PtrInfo* p_ptrInfo) : m_pPtrInfo(p_ptrInfo)
	  {
		 if (m_pPtrInfo) { m_pPtrInfo->increment(); }
	  }

	  WeakPtr(const WeakPtr& other) : m_pPtrInfo(other.m_pPtrInfo)
	  {
		  if (m_pPtrInfo)
		  {
		    m_pPtrInfo->increment();
		  }
		}

		WeakPtr& operator=(const WeakPtr& other)
		{
			PtrInfo::decrement(m_pPtrInfo);
			m_pPtrInfo = other.m_pPtrInfo;
			if (m_pPtrInfo)
			{
				m_pPtrInfo->increment();
			}
			return *this;
		}

		void clear()
		{
			PtrInfo::decrement(m_pPtrInfo);
			m_pPtrInfo = nullptr;
		}

		template <typename U>
		WeakPtr<U> getUpCast()
		{
			static_assert(std::is_base_of<U, T>::value, ""); // UがTの基底クラス
			return WeakPtr<U>(m_pPtrInfo);
		}

		template <typename U>
		WeakPtr<U> getDownCast()
		{
			static_assert(std::is_base_of<T, U>::value, ""); // UはTの派生クラス
			if (dynamic_cast<U*>(getPtr()) // ポインタが実際にU型インスタンスであるかチェック
			{
				return WeakPtr<U>(m_pPtrInfo);
			}
			return WeakPtr<U>(nullptr); // ダウンキャストに失敗したらNULLポインタのWeakPtrを返す
		}

		bool isNull() const
		{
			return !m_pPtrInfo || m_pPtrInfo->isNull();
		}

		T* getPtr() const
		{
			return m_pPtrInfo ? m_pPtrInfo->getPtr<T> : nullptr;
		}

		T& operator*() const
		{
			assert(!isNull());
			return *getPtr();
		}

		T* operator->() const
		{
			asset(!isNull());
			return getPtr();
		}

		operator T* () const
		{
			return getPtr();
		}

		size_t getRefCount() const
		{
			return m_pPtrInfo ? m_pPtrInfo->getRefCount() : 0;
		}
	};
  
   
   /*ポインタ情報クラス*/
	class PtrInfo
	{
	public:
		void* m_ptr{ nullptr };
		size_t m_refCount{ 0 };

		PtrInfo(void* ptr) : m_ptr(ptr) {}

		void increment() { ++m_refCount; }

		static void decrement(PtrInfo*& p_info) // ポインタ型の参照
		{
			if (!p_info) { return; }
			--p_info->m_refCount;
			if (p_info->m_refCount == 0)
			{
				delete p_info;
				p_info = nullptr;
			}
		}

		bool isNull() const { return m_ptr == nullptr; }

		template <typename T>
		T* getPtr() const
		{
			return reinterpret_cast<T*>(m_ptr);
		}

		size_t getRefCount() const
		{
			return m_refCount;
		}
	};
  
  
  /*弱参照ポインタ管理クラス*/
	template <typename T>
	class WeakPtrController
	{
		WeakPtr<T> m_weakPtr;

	public:
		~WeakPtrController()
		{
			if (m_weakPtr.m_pPtrInfo)
			{
				m_weakPtr.m_pPtrInfo->m_ptr = nullptr;
			}
		}

		explicit WeakPtrController(T* ptr)
			: m_weakPtr(new PtrInfo(ptr))
		{}

		WeakPtrController() = delete;
		WeakPtrController(const WeakPtrController& other) = default;
		WeakPtrController& operator=(const WeakPtrController& other) = default;

		template <typename U>
		WeakPtr<U> getDownCastUnsafe(U* p_this)
		{
			(void)p_this;
			static_assert(std::is_base_of<T, U>::value "");
			return WeakPtr<U>
		}

		WeakPtr<T> getWeakPtr() { return m_weakPtr; }
	};

#define DEFINE_WEAK_CONTROLLER(TYPE)                                               \
	public: WeakPtr<TYPE> getWeakPtr() { return m_WeakPtrController.getWeakPtr(); }  \
	protected: WeakPtrController<TYPE> m_WeakPtrController{this}

#define DEFINE_WEAK_GET(TYPE)  \
	public: WeakPtr<TYPE> getWeakPtr( void ) { return m_WeakPtrController.GetDowncasted_unsafe( this ); } class{}

}
