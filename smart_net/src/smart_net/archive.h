/*
 * archive.h
 *
 *  Created on: Apr 24, 2009
 *      Author: rock
 */

#ifndef __ARCHIVE_H__
#define __ARCHIVE_H__

#include <deque>
#include "mem.h"
#include "pkg.h"

#ifdef __USING_COMPRESSED_DATA__
#include "zlib_wrapper.h"
#endif

namespace nm_pkg
{
	using namespace nm_utils;

	//package header + package body + package body + ...
	//#define BYTES_DATA_LEN sizeof(u_int32_t)

	typedef std::deque<nm_mem::mem_ptr_t> mem_queue_t;

	///为了使用高效使用可靠组播协议，本版本不支持queue mem队列了:<
	template<typename H/*package header*/, typename B/*package body*/>
	class CArchive
	{
	public:
		CArchive(int32_t i32Cnt = -1/*estimated value*/) :
			m_pHdr(NULL), m_i32Cnt((-1 == i32Cnt) ? __MAX_MEM_SIZE__ : i32Cnt), m_bPending(false)
		{
		}
		~CArchive()
		{
		}

		enum
		{
			HdrSize = sizeof(H),
		//PkgSize = sizeof(B)
		};

	public:
		H* get_hdr(cmn_byte_t bVer = PKG_VER/*, byte_t bChk = CHECK_CODE*/)
		{
			if (NULL != m_pHdr)
			{
				return m_pHdr;
			}

			//static const u_int32_t s_ui32MaxPkgSize = B::get_max_size();
			CMN_ASSERT(NULL == m_pMem);
			m_pMem = NEW_MEM(((HdrSize + s_ui32MaxPkgSize * m_i32Cnt) > __MAX_MEM_SIZE__ ? __MAX_MEM_SIZE__ : (HdrSize + s_ui32MaxPkgSize * m_i32Cnt)));
			m_pHdr = new (m_pMem->get_tail_free_buf()) H(B::PKG_OPCODE, 0, bVer/*, bChk*/);
			m_pMem->inc_len(HdrSize);
			//m_pMem->inc_offset(HdrSize);
			//m_bPending = false;

			return m_pHdr;
		}

		B* get_next_body()
		{
			///
			if (NULL == m_pHdr)
			{
				(void)get_hdr();
			}

			///将上个消息体的信息补充下
			body_ready();
//			///
			static const u_int32_t uiMaxPkgSize = B::get_max_size();
			if (uiMaxPkgSize > m_pMem->get_tail_free_size())
			{
				return NULL;
			}

			if (0 == --m_i32Cnt)
			{
				m_i32Cnt = 1;
			}

			m_bPending = true;
			//return *pB;
			return (B*)(m_pMem->get_tail_free_buf());
		}

		u_int32_t get_len()
		{
			return (NULL == m_pMem) ? 0 : m_pMem->get_len();
		}

#ifdef __USING_COMPRESSED_DATA__
		mem_queue_t& serialize(bool bCompressed = true, bool bUdp = false)
#else
		nm_mem::mem_ptr_t& serialize(bool bUdp = false)
#endif
		{
			///将最后一个消息体的消息补全
			body_ready();

//			if (NULL != m_pMem)
//			{
//				m_pMem->set_offset(0);
//				m_uiLen += m_pMem->get_len();
//				m_qMem.push_back(m_pMem);
//				m_pMem = NULL;
//			}

//			if (bUdp)
//			{
//				CMN_ASSERT(m_qMem.size() == 1);
//			}

			CMN_ASSERT(m_pHdr != NULL);
			m_pHdr->set_len(m_pMem->get_len());
			m_pHdr->set_type(NORMAL_PKG);
#ifdef __USING_COMPRESSED_DATA__
			if (bCompressed && (m_uiLen > __PKG_COMPRESSED_THRESHOLD__))
			{
				CMN_ASSERT(m_queue_compressed.empty());
				CMN_ASSERT(nm_zlib::compress(m_qMem, m_queue_compressed, -1/*Z_DEFAULT_COMPRESSION*/) >= 0);
				m_qMem.clear();
				return m_queue_compressed;
			}
			else
			{
				return m_qMem;
			}
#else
			return m_pMem;
#endif
		}

	private:
		///将上个消息体的信息补充完善
		void body_ready()
		{
			if (!m_bPending)
			{
				return;
			}

			B *pB = (B*) (m_pMem->get_tail_free_buf());
			m_pMem->inc_len(pB->get_real_size());
			//m_pMem->inc_offset(pB->get_real_size());
			m_bPending = false;
		}

	private:
		H *m_pHdr;
		int32_t m_i32Cnt; //estimate how many package(body).
		bool m_bPending; //one package is ready
		nm_mem::mem_ptr_t m_pMem;
#ifdef __USING_COMPRESSED_DATA__
		mem_queue_t m_queue_compressed;
#endif
	};
}

#endif /* __ARCHIVE_H__ */
