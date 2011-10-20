/*
 * mem.h
 *
 *  Created on: Apr 5, 2009
 *      Author: rock
 */

#ifndef __MEM_H__
#define __MEM_H__

#include <deque>

#include "type.h"
#include "base.h"
#include "singleton.h"
#include "smart_ptr.h"
#include "objs_pool.h"

namespace nm_utils
{
#define LEVEL_ONE_MEM (64)
#define LEVEL_TWO_MEM (4*LEVEL_ONE_MEM)
#define LEVEL_THREE_MEM (4*LEVEL_TWO_MEM)
#define LEVEL_FOUR_MEM (4*LEVEL_THREE_MEM)
#define LEVEL_FIVE_MEM (4*LEVEL_FOUR_MEM)
#define MAX_MEM_SIZE LEVEL_FIVE_MEM
#define NEW_MEM(size) CMemMgr::get_mem(size)

	class CBaseMem : public CRefCnt
	{
	public:
		CBaseMem():m_uiLen(0), m_uiOffset(0){}
		virtual ~CBaseMem(){}

	public:
		virtual ui32_t get_max_size() = 0;
		virtual byte_t* get_buf() = 0;

		byte_t* get_cur_buf()
		{
			return get_buf() + m_uiOffset;
		}

		ui32_t get_cur_len()
		{
			return m_uiLen - m_uiOffset;
		}

		ui32_t get_offset()
		{
			return m_uiOffset;
		}

		void set_offset(ui32_t uiOffset)
		{
			m_uiOffset = uiOffset;
		}

		void inc_offset(ui32_t uiIncVal)
		{
			m_uiOffset += uiIncVal;
		}

		void set_len(ui32_t uiSize)
		{
			m_uiLen = uiSize;
		}

		void inc_len(ui32_t uiSize)
		{
			m_uiLen += uiSize;
		}

		ui32_t get_len()
		{
			return m_uiLen;
		}

		ui32_t get_free_size()
		{
			return get_max_size()-m_uiLen;
		}

		byte_t* get_free_buf()
		{
			return get_buf() + m_uiLen;
		}

		void append(byte_t *pBytes, ui32_t uiBytes)
		{
			ASSERT(get_free_size() >= uiBytes);
			::memcpy(get_buf()+m_uiLen, pBytes, uiBytes);
			m_uiLen += uiBytes;
		}

		void rearrange()
		{
			if (0 == m_uiOffset)
			{
				return;
			}

			ui32_t ui = get_cur_len();
			::memmove(get_buf(), get_cur_buf(), ui);
			m_uiOffset = 0;
			m_uiLen = ui;
		}

		void init()
		{
			m_uiOffset = 0;
			m_uiLen = 0;
		}

		void reclaim()
		{
			m_uiOffset = 0;
			m_uiLen = 0;
		}

		//pay attention
//		byte_t* operator &()
//		{
//			return get_buf();
//		}

	protected:
		ui32_t m_uiOffset;  //offset
		ui32_t m_uiLen; 	//total length, include the data before offset.
	};

	template<ui32_t uiMaxSize>
	class CMem: public CBaseMem
	{
	public:
		ui32_t get_max_size()
		{
			return uiMaxSize;
		}
		byte_t* get_buf()
		{
			return m_bytes;
		}

		void init(CBaseObjsPool<CMem<uiMaxSize> > *pObjsPool)
		{
			CBaseMem::init();
			m_pObjsPool = pObjsPool;
		}

		void dispose_this_obj()
		{
			m_pObjsPool->set_obj(this);
		}

	private:
		byte_t m_bytes[uiMaxSize];
		CBaseObjsPool<CMem<uiMaxSize> > *m_pObjsPool;
	};

	class CMemMgr : public nonnewobj
	{
	public:
		static inline CBaseMem* get_mem(ui32_t uiMemSize)
		{
			if (uiMemSize < LEVEL_ONE_MEM)
			{
				return CSingleton<CObjsPool<CMem<LEVEL_ONE_MEM> > >::instance().get_obj();
			}
			else if (uiMemSize < LEVEL_TWO_MEM)
			{
				return CSingleton<CObjsPool<CMem<LEVEL_TWO_MEM> > >::instance().get_obj();
			}
			else if (uiMemSize < LEVEL_THREE_MEM)
			{
				return CSingleton<CObjsPool<CMem<LEVEL_THREE_MEM> > >::instance().get_obj();
			}
			else if (uiMemSize < LEVEL_FOUR_MEM)
			{
				return CSingleton<CObjsPool<CMem<LEVEL_FOUR_MEM> > >::instance().get_obj();
			}
			else if (uiMemSize <= LEVEL_FIVE_MEM)
			{
				return CSingleton<CObjsPool<CMem<LEVEL_FIVE_MEM> > >::instance().get_obj();
			}
			else
			{
				ASSERT(false);
				return NULL;
			}
		}
	};

	typedef nm_utils::CSmartPtr<CBaseMem> mem_ptr_t;
	typedef std::deque<mem_ptr_t> mem_queue_t;
}

#endif /* __MEM_H__ */
