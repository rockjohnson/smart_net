/*
 * ring_buf.h
 *
 *  Created on: Nov 19, 2011
 *      Author: rock
 */

#ifndef RING_BUF_H_
#define RING_BUF_H_

#include "../common/common.h"
#include "../utils/smart_lock.h"

namespace nm_mem
{

#define CACHE_LINE_SIZE (128)
#define DEF_BLOCK_SIZE (CACHE_LINE_SIZE * 20)

	/**
	 *
	 * ring buffer...
	 *
	 * */
	class CSmartRB
	{

	public:
		CSmartRB(u_int32_t ui32BlockCnt, u_int32_t ui32BlockSz = DEF_BLOCK_SIZE) :
			m_ui32BlockCnt(ui32BlockCnt), m_ui32BlockSz(ui32BlockSz)
		{
			m_bytes = malloc(ui32BlockCnt * ui32BlockSz);
			CMN_ASSERT(NULL != m_bytes);
			reset();
		}

		void reset()
		{
			CMN_ASSERT(NULL != m_bytes);
			ZERO_MEM(m_bytes, m_ui32BlockCnt * m_ui32BlockSz);
			m_ui64DataHeadIdx = 1;
			m_ui64FreeHeadIdx = 1;
		}

		~CSmartRB()
		{
			free(m_bytes);
		}

		u_int32_t get_block_cnt()
		{
			return m_ui32BlockCnt;
		}

		u_int32_t get_block_sz()
		{
			return m_ui32BlockSz;
		}

	public:
		/**
		 *
		 * */
		bool empty()
		{
			return (m_ui64DataHeadIdx == m_ui64FreeHeadIdx);
		}

		/**
		 *
		 * */
		bool full()
		{
			return ((m_ui64FreeHeadIdx - m_ui64DataHeadIdx) == m_ui32BlockCnt);
		}

		/**
		 *
		 * */
		u_int64_t get_data_head_idx()
		{
			return m_ui64DataHeadIdx;
		}

		/**
		 * increase....
		 * */
		void set_data_head_idx(u_int64_t ui64Idx)
		{
			CMN_ASSERT(ui64Idx > m_ui64DataHeadIdx && ui64Idx < m_ui64FreeHeadIdx);
			m_ui64DataHeadIdx = ui64Idx;
		}

		/**
		 *
		 * */
		u_int64_t get_free_head_idx()
		{
			return m_ui64FreeHeadIdx;
		}

		/**
		 *
		 * */
		void set_free_head_idx(u_int64_t ui64Idx)
		{
			CMN_ASSERT(ui64Idx > m_ui64FreeHeadIdx && ((ui64Idx - m_ui64DataHeadIdx) <= m_ui32BlockCnt));
			m_ui64FreeHeadIdx = ui64Idx;
		}

		/**
		 * called by writer thread
		 *
		 * not thread safe....should be called with add_tail_data();
		 *
		 * */
		cmn_byte_t* get_free_head()
		{
			return full() ? NULL : (m_bytes + (m_ui64FreeHeadIdx % m_ui32BlockCnt) * m_ui32BlockSz);
		}

		/**
		 * only be called by writer thread.
		 * yeah, this func is thread safe. you can call this func multithread.
		 * return 0 means failed.
		 * */
		u_int64_t set_data(cmn_byte_t *pData, u_int32_t ui32Len)
		{
			CMN_ASSERT(m_ui32BlockSz >= ui32Len);
			if (full())
			{
				///full, no free buf
				return 0;
			}
			memcpy(m_bytes + (m_ui64FreeHeadIdx % m_ui32BlockCnt) * m_ui32BlockSz, pData, ui32Len);

			return (m_ui64FreeHeadIdx++);
		}

		/**
		 * called by reader thread...
		 * */
		cmn_byte_t* get_data(u_int64_t ui64Idx)
		{
			if (empty() || (ui64Idx >= m_ui64FreeHeadIdx) || (ui64Idx < m_ui64DataHeadIdx))
			{
				///no data...
				CMN_ASSERT(false);
				return NULL;
			}

			return (m_bytes + (ui64Idx % m_ui32BlockCnt) * m_ui32BlockSz);
		}

	private:
		cmn_byte_t *m_bytes;
		volatile u_int64_t m_ui64DataHeadIdx; ///first data index. where u can read the first data
		volatile u_int64_t m_ui64FreeHeadIdx; ///first free buffer index. where u can write the first data
		u_int32_t m_ui32BlockCnt;
		u_int32_t m_ui32BlockSz;
	};

/**
 *
 *  e.g.1
 *
 *  writer thread:
 *
 *  u_int64_t ui64SeqNo = 0;
 *  cmn_byte_t *pBuf = rb.get_free(ui64Seq);
 *  CPkgHdr *pHdr = (CPkgHdr*)pBuf;
 *  pHdr->ui64SeqNo = ui64SeqNo;
 *  rb.inc_tail_data();
 *
 *  reader thread:
 *
 *  u_int32_t ui32Len = 0;
 *	cmn_byte_t *pData = rm.get_data(m_ui64NextSeqNo++);
 *	send_data(pData);
 *  rb.dec_head_data();
 *
 *
 *
 *
 * */

}

#endif /* RING_BUF_H_ */
