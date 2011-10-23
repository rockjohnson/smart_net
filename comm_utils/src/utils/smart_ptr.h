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
			CMN_ASSERT(m_iCnt == 0);
		}

	public:
		void inc()
		{
			atomic_increment(&m_iCnt);
		}

		void dec()
		{
			CMN_ASSERT(m_iCnt> 0);
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
		/*volatile*/
		int m_iCnt;
	};
	//#define __SUPPORT_REF_CNT__ : public nm_utils::CRefCnt


	/**
	 *
	 * */
	template<typename T>
	class CSmartPtr
	{
	public:
		inline CSmartPtr(T *pObj = 0) :
			m_pObj(pObj)
		{
			inc_ref();
		}

		inline CSmartPtr(const CSmartPtr<T> &ptr) :
			m_pObj(ptr.get_ptr())
		{
			inc_ref();
		}

		template<typename TT>
		inline CSmartPtr(const CSmartPtr<TT> &ptr) :
			m_pObj(ptr.get_ptr())
		{
			inc_ref();
		}

		inline ~CSmartPtr()
		{
			dec_ref();
		}

	public:
		inline T* operator ->() const
		{
			return m_pObj;
		}

		inline T& operator *()
		{
			return *m_pObj;
		}

		inline T* get_ptr() const
		{
			return m_pObj;
		}

		inline bool operator !=(T *pObj)
		{
			return m_pObj != pObj;
		}

		inline bool operator ==(T *pObj)
		{
			return m_pObj == pObj;
		}

		inline bool operator !=(const CSmartPtr<T> &ptr)
		{
			return m_pObj != ptr.m_pObj;
		}

		inline CSmartPtr<T>& operator =(const CSmartPtr<T> &ptr)
		{
			if (m_pObj != ptr.m_pObj)
			{
				dec_ref();
				m_pObj = ptr.m_pObj;
				inc_ref();
			}

			return *this;
		}

		inline CSmartPtr<T>& operator =(T *pObj)
		{
			if (m_pObj != pObj)
			{
				dec_ref();
				m_pObj = pObj;
				inc_ref();
			}

			return *this;
		}

		inline operator T*() ///now you can do this: if (NULL == CSmartPtr<T>){....}
		{
			return m_pObj;
		}

		inline operator size_t() const ///for unordered_set
		{
			return (size_t) m_pObj;
		}

	private:
		inline void inc_ref()
		{
			if (m_pObj != 0)
			{
				m_pObj->inc_ref();
			}
		}

		inline void dec_ref()
		{
			if (m_pObj != 0)
			{
				m_pObj->dec_ref();
				m_pObj = 0;
			}
		}

	private:
		T *m_pObj;

		template<typename TT>
		friend bool operator ==(const CSmartPtr<TT> &one, const CSmartPtr<TT> &two);
		template<typename TT>
		friend bool operator ==(const CSmartPtr<TT> &one, const void *pVoid);
		template<typename TT>
		friend bool operator !=(const CSmartPtr<TT> &one, const void *pVoid);
		template<typename TT>
		friend bool operator <(const CSmartPtr<TT> &one, const CSmartPtr<TT> &two);
	};

	template<typename T>
	inline bool operator ==(const CSmartPtr<T> &one, const CSmartPtr<T> &two)
	{
		return one.m_pObj == two.m_pObj;
	}

	template<typename T>
	inline bool operator ==(const CSmartPtr<T> &one, const void *pVoid)
	{
		return one.m_pObj == pVoid;
	}

	template<typename T>
	inline bool operator !=(const CSmartPtr<T> &one, const void *pVoid)
	{
		return one.m_pObj != pVoid;
	}

	template<typename TT>
	inline bool operator <(const CSmartPtr<TT> &one, const CSmartPtr<TT> &two)
	{
		return one.m_pObj < two.m_pObj;
	}

#if 0
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
#endif

	template<typename DST, typename SRC>
	inline CSmartPtr<DST> dynamic_cast_smartptr(const CSmartPtr<SRC> &pSrc)
	{
#ifdef __CHECK_CAST__
		return CSmartPtr<DST>(dynamic_cast<DST*>(pSrc.get_ptr()));
#else
		return CSmartPtr<DST> (reinterpret_cast<DST*> (pSrc.get_ptr())); //god!help me!!
#endif
	}
}

#endif /* __SMART_PTR_H__ */
