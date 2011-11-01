/*
 * noncopyable.h
 *
 *  Created on: Apr 5, 2009
 *      Author: rock
 */

#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

#include "types.h"
#include "defines.h"
#include "../utils/atomic.h"

namespace nm_cmn_base
{
	class CNoncopyable
	{
	protected:
		CNoncopyable()
		{
		}
		virtual ~CNoncopyable()
		{
		}
	private:
		// emphasize the following members are private
		CNoncopyable(const CNoncopyable&);
		const CNoncopyable& operator=(const CNoncopyable&);
	};

	class CNonnewobj
	{
	private:
		CNonnewobj()
		{
		}
		CNonnewobj(const CNonnewobj &t)
		{
		}
	public:
		virtual ~CNonnewobj()
		{
		}
	};

	/**
	 * common base
	 * */
	class ICommonBase
	{
	protected:
		inline ICommonBase() :
			m_iCnt(0)
		{
		}

		inline virtual ~ICommonBase()
		{
			CMN_ASSERT(m_iCnt == 0);
		}

	DISALLOW_COPY_AND_ASSIGN(ICommonBase);

	public:
		inline void inc_ref()
		{
			nm_utils::atomic_increment(&m_iCnt);
		}

		inline void dec_ref()
		{
			CMN_ASSERT(m_iCnt> 0);
			if (nm_utils::atomic_exchange_and_add(&m_iCnt, -1) == 1)
			{
				dispose_this_obj();
			}
		}

		inline void dispose_this_obj()
		{
			delete this;
		}

		inline int get_ref_cnt()
		{
			return m_iCnt;
		}

	private:
		int32_t m_iCnt;
	};
}

#endif /* __NONCOPYABLE_H__ */
