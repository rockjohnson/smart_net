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

#define MAX_MEM_SIZE (8192*4)

namespace nm_mem
{

	class CMemBlock: public nm_cmn_base::ICommonBase
	{
	public:
		inline CMemBlock(u_int32_t ui32Offset = 0)
		:m_ui32InitOffset(ui32Offset), m_ui32Sz(sizeof(m_bytes)), m_ui32Offset(ui32Offset), m_ui32Len(0)
		{
		}
		inline ~CMemBlock()
		{
		}

	public:
		inline cmn_byte_t* get_data()
		{
			return (m_bytes + m_ui32Offset);
		}
		inline cmn_byte_t* get_offset_data(u_int32_t ui32Offset)
		{
			return (m_bytes + ui32Offset);
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
				::memmove(m_bytes, m_bytes + m_ui32Offset, m_ui32Len);
				m_ui32Offset = 0;
			}
		}
		inline void dec_head_data(u_int32_t ui32Len)
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
		inline void inc_offset(u_int32_t ui32Len)
		{
			m_ui32Offset += ui32Len;
		}
		inline void dec_len(u_int32_t ui32Len)
		{
			m_ui32Len -= ui32Len;
		}
		inline cmn_byte_t* get_tail_free_buf()
		{
			return (m_bytes + m_ui32Offset + m_ui32Len);
		}
		inline u_int32_t get_total_free_size()
		{
			return (m_ui32Sz - m_ui32Len);
		}
		inline cmn_byte_t* get_buf()
		{
			return m_bytes;
		}
		inline u_int32_t get_init_offset()
		{
			return m_ui32InitOffset;
		}

		inline int32_t append(cmn_byte_t* pBytes, u_int32_t ui32Len)
		{
			if (get_total_free_size() < ui32Len)
			{
				return CMNERR_COMMON_ERR;
			}
			///
			if (get_tail_free_size() < ui32Len)
			{
				move_data_ahead();
			}
			///
			memcpy(m_bytes+m_ui32Offset+m_ui32Len, pBytes, ui32Len);
			m_ui32Len += ui32Len;

			return CMNERR_SUC;
		}

		inline int32_t inc_head_data(u_int32_t ui32Len)
		{
			if (ui32Len > m_ui32Offset)
			{
				return CMNERR_COMMON_ERR;
			}

			m_ui32Offset -= ui32Len;
			m_ui32Len += ui32Len;
			return CMNERR_SUC;
		}

		inline void reset(bool bUseInitOffset = false)
		{
			//SAFE_DELETE_ARR(m_pBytes);
			//m_ui32Sz = MAX_MEM_SIZE;
			m_ui32Offset = bUseInitOffset ? m_ui32InitOffset : false;
			m_ui32Len = 0;
		}

	private:
		cmn_byte_t m_bytes[MAX_MEM_SIZE];
		const u_int32_t m_ui32InitOffset;
		u_int32_t m_ui32Sz;
		u_int32_t m_ui32Offset;
		u_int32_t m_ui32Len;
	};
	typedef nm_utils::CSmartPtr<nm_mem::CMemBlock> mem_ptr_t;

#define NEW_MEM(v) \
		SYS_NOTRW_NEW(nm_mem::CMemBlock)

}

#endif /* MEM_H_ */
