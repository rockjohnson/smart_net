/*
 * mem.h
 *
 *  Created on: Feb 12, 2011
 *      Author: rock
 */

#ifndef __MEM_H__
#define __MEM_H__

#include "../common/common.h"
#include "../utils/smart_ptr.h"

namespace nm_mem
{

	class CMemBlock: public nm_cmn_base::ICommonBase
	{
	public:
		inline CMemBlock(){}
		inline ~CMemBlock()
		{
			reset();
		}

	public:
		inline cmn_byte_t* get_data()
		{
			return (m_pBytes + m_ui32Offset);
		}
		inline u_int32_t get_len()
		{
			return m_ui32Len;
		}
		inline u_int32_t get_offset()
		{
			return m_ui32Offset;
		}
		inline void move_data_ahead()
		{
			if (m_ui32Offset > 0)
			{
				::memmove(m_pBytes, m_pBytes + m_ui32Offset, m_ui32Len);
			}
		}
		inline void move_head_data(u_int32_t ui32Len)
		{
			m_ui32Offset += ui32Len;
			m_ui32Len -= ui32Len;
		}
		inline u_int32_t get_tail_free_size()
		{
			return (m_ui32Sz - (m_ui32Len + m_ui32Offset));
		}
		inline void inc_len(u_int32_t ui32Len)
		{
			m_ui32Len += ui32Len;
		}
		inline cmn_byte_t* get_tail_free_buf()
		{
			return (m_pBytes + m_ui32Offset + m_ui32Len);
		}
		inline u_int32_t get_total_free_size()
		{
			return (m_ui32Sz - m_ui32Len);
		}
		inline u_int32_t get_cur_len()
		{
			return m_ui32Len;
		}
		inline cmn_byte_t* get_cur_buf()
		{
			return (m_pBytes + m_ui32Offset);
		}
		inline cmn_byte_t* get_buf()
		{
			return m_pBytes;
		}

		inline int32_t append(cmn_byte_t* pBytes, u_int32_t ui32Len)
		{
			if (get_total_free_size() < ui32Len)
			{
				return CMNERR_COMMON_ERR;
			}

			if (get_tail_free_size() < ui32Len)
			{
				memmove(m_pBytes, m_pBytes+m_ui32Offset, m_ui32Len);
				m_ui32Offset = 0;
			}
			memcpy(m_pBytes+m_ui32Offset+m_ui32Len, pBytes, ui32Len);
			m_ui32Len += ui32Len;

			return CMNERR_SUC;
		}

		inline void reset()
		{
			//SAFE_DELETE_ARR(m_pBytes);
			m_ui32Sz = 0;
			m_ui32Offset = 0;
			m_ui32Len = 0;
		}

	private:
		cmn_byte_t m_pBytes[8192*4];
		u_int32_t m_ui32Sz;
		u_int32_t m_ui32Offset;
		u_int32_t m_ui32Len;
	};
	typedef nm_utils::CSmartPtr<nm_mem::CMemBlock> mem_ptr_t;

#define NEW_MEM() \
		SYS_NOTRW_NEW(nm_mem::CMemBlock)

}

#endif /* MEM_H_ */
