/*
 * smart_ptr.h
 *
 *  Created on: Apr 3, 2009
 *      Author: rock
 */

#ifndef __SMART_PTR_H__
#define __SMART_PTR_H__

#include <assert.h>

#include "atomic.h"

namespace nm_utils
{
	class CRefCnt
	{
	protected:
		CRefCnt() :
			m_iCnt(0)
		{
		}

		virtual ~CRefCnt()
		{
			SYS_ASSERT(m_iCnt == 0);
		}

	public:
		void inc()
		{
			atomic_increment(&m_iCnt);
		}

		void dec()
		{
			SYS_ASSERT(m_iCnt> 0);
			if (atomic_exchange_and_add(&m_iCnt, -1) == 1)
			{
				dispose_this_obj();
			}
		}

		virtual void dispose_this_obj()
		{
			delete this;
		}

		int get_ref_cnt()
		{
			return m_iCnt;
		}

	private:
		CRefCnt(const CRefCnt &other)
		{
		}
		CRefCnt& operator =(const CRefCnt&)
		{
			return *this;
		}

	private:
		/*volatile*/int m_iCnt;
	};
//#define __SUPPORT_REF_CNT__ : public nm_utils::CRefCnt


	template<typename T>
	class CSmartPtr
	{
	public:
		CSmartPtr(T *pObj = 0);
		template<typename TT>
		CSmartPtr(const CSmartPtr<TT> &ptr)
		:m_pObj(ptr.get_ptr())
		{
			inc_ref();
		}
		~CSmartPtr();

	public:
		T* operator ->() const;
		T& operator *();
		T* get_ptr() const;
		bool operator !=(T *pObj);
		bool operator ==(T *pObj);
		bool operator !=(const CSmartPtr<T> &ptr);
		CSmartPtr<T>& operator =(const CSmartPtr<T> &ptr);
		CSmartPtr<T>& operator =(T *pObj);
		operator T*(); //now you can do this: if (NULL == CSmartPtr<T>){....}

		operator size_t() const;//for unordered_set

	private:
		inline void inc_ref();
		inline void dec_ref();

		T *m_pObj;

		template<typename TT>
		friend bool operator ==(const CSmartPtr<TT> &one, const CSmartPtr<TT> &two);
		template<typename TT>
		friend bool operator ==(const CSmartPtr<TT> &one, const void *pVoid);
		template<typename TT>
		friend bool operator !=(const CSmartPtr<TT> &one, const void *pVoid);
		template <typename TT>
		friend bool operator < (const CSmartPtr<TT> &one, const CSmartPtr<TT> &two);
	};

	template<typename T>
	bool operator ==(const CSmartPtr<T> &one, const CSmartPtr<T> &two)
	{
		return one.m_pObj == two.m_pObj;
	}

	template<typename T>
	bool operator ==(const CSmartPtr<T> &one, const void *pVoid)
	{
		return one.m_pObj == pVoid;
	}

	template<typename T>
	bool operator !=(const CSmartPtr<T> &one, const void *pVoid)
	{
		return one.m_pObj != pVoid;
	}

	template <typename TT>
	bool operator < (const CSmartPtr<TT> &one, const CSmartPtr<TT> &two)
	{
		return one.m_pObj < two.m_pObj;
	}

	template<typename T>
	CSmartPtr<T>::CSmartPtr(T *pObj) :
		m_pObj(pObj)
	{
		inc_ref();
	}

//	template<typename T>
//	CSmartPtr<T>::CSmartPtr(const CSmartPtr<T> &ptr) :
//		m_pObj(ptr.m_pObj)
//	{
//		inc_ref();
//	}

	template<typename T>
	CSmartPtr<T>::~CSmartPtr()
	{
		dec_ref();
	}

	template<typename T>
	T* CSmartPtr<T>::operator ->() const
	{
		return m_pObj;
	}

	template<typename T>
	T& CSmartPtr<T>::operator *()
	{
		return *m_pObj;
	}

	template<typename T>
	T* CSmartPtr<T>::get_ptr() const
	{
		return m_pObj;
	}

	template<typename T>
	bool CSmartPtr<T>::operator !=(T *pObj)
	{
		return m_pObj != pObj;
	}

	template<typename T>
	bool CSmartPtr<T>::operator !=(const CSmartPtr<T> &ptr)
	{
		return m_pObj != ptr.m_pObj;
	}

	template<typename T>
	bool CSmartPtr<T>::operator ==(T *pObj)
	{
		return m_pObj == pObj;
	}

	template<typename T>
	CSmartPtr<T>& CSmartPtr<T>::operator =(const CSmartPtr<T> &ptr)
	{
		if (m_pObj != ptr.m_pObj)
		{
			dec_ref();
			m_pObj = ptr.m_pObj;
			inc_ref();
		}

		return *this;
	}

	template<typename T>
	CSmartPtr<T>& CSmartPtr<T>::operator =(T *pObj)
	{
		if (m_pObj != pObj)
		{
			dec_ref();
			m_pObj = pObj;
			inc_ref();
		}

		return *this;
	}

	template <typename T>
	CSmartPtr<T>::operator T*()
	{
		return m_pObj;
	}

	template <typename T>
	CSmartPtr<T>::operator size_t() const
	{
		return (size_t)m_pObj;
	}

	template<typename T>
	void CSmartPtr<T>::inc_ref()
	{
		if (m_pObj != 0)
		{
			m_pObj->inc_ref();
		}
	}

	template<typename T>
	void CSmartPtr<T>::dec_ref()
	{
		if (m_pObj != 0)
		{
			m_pObj->dec_ref();
			m_pObj = 0;
		}
	}

	template <typename T, typename TT>
	CSmartPtr<T> dynamic_cast_smartptr(CSmartPtr<TT> &pTT)
	{
		return CSmartPtr<T>(dynamic_cast<T*>(pTT.get_ptr()));
	}
}

#endif /* __SMART_PTR_H__ */
