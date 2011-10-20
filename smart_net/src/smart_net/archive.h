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
	//using namespace nm_network;

	//package header + package body + package body + ...
//#define BYTES_DATA_LEN sizeof(ui32_t)

	template <typename H/*package header*/, typename B/*package body*/>
	class CArchive
	{
	public:
		CArchive(us16_t usCnt/*estimated value*/)
		: m_pHdr(NULL), m_usCnt(usCnt), m_uiLen(0), m_bPending(false){}
		virtual ~CArchive(){}

		enum
		{
			HdrSize = sizeof(H),
			//PkgSize = sizeof(B)
		};

	public:
		H& get_hdr(byte_t bVer = VERSION/*, byte_t bChk = CHECK_CODE*/)
		{
			if (m_pHdr != NULL)
			{
				return *m_pHdr;
			}

			static const ui32_t uiMaxPkgSize = B::get_max_size();
			ASSERT(NULL == m_pMem);
			ASSERT(m_queue_mem.empty());
			m_pMem = NEW_MEM(((HdrSize+uiMaxPkgSize*m_usCnt) > MAX_MEM_SIZE
					? MAX_MEM_SIZE : (HdrSize+uiMaxPkgSize*m_usCnt)));
			m_pHdr = new (m_pMem->get_cur_buf())H(B::PKG_OPCODE, 0, bVer/*, bChk*/);
			m_pMem->inc_len(HdrSize);
			m_pMem->inc_offset(HdrSize);
			m_bPending = false;

			return *m_pHdr;
		}

		B& get_next_body()
		{
			if (m_pHdr == NULL)
			{
				get_hdr();
			}

			body_ready();

			static const ui32_t uiMaxPkgSize = B::get_max_size();
			ASSERT(uiMaxPkgSize <= MAX_MEM_SIZE);  //attention!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
			ASSERT(NULL != m_pMem);
			if (m_pMem->get_free_size() < uiMaxPkgSize)
			{
				m_pMem->set_offset(0); //for sending
				m_uiLen += m_pMem->get_len();
				m_queue_mem.push_back(m_pMem);
				m_pMem = NEW_MEM(((uiMaxPkgSize*m_usCnt) > MAX_MEM_SIZE ? MAX_MEM_SIZE : (uiMaxPkgSize*m_usCnt)));
			}

			//B *pB = (B*)(m_pMem->get_cur_buf());
			//m_pMem->inc_len(PkgSize);
			//m_pMem->inc_offset(PkgSize);

			if (0 == --m_usCnt)
			{
				m_usCnt = 1;
			}

			m_bPending = true;
			//return *pB;
			return *((B*)(m_pMem->get_cur_buf()));
		}

		ui32_t get_len()
		{
			return m_uiLen;
		}

#ifdef __USING_COMPRESSED_DATA__
		mem_queue_t& serialize(bool bCompressed = true, bool bUdp = false)
#else
		mem_queue_t& serialize(bool bUdp = false)
#endif
		{
			body_ready();

			if (NULL != m_pMem)
			{
				m_pMem->set_offset(0);
				m_uiLen += m_pMem->get_len();
				m_queue_mem.push_back(m_pMem);
				m_pMem = NULL;
			}

			if (bUdp)
			{
				ASSERT(m_queue_mem.size() == 1);
			}

			ASSERT(m_pHdr != NULL);
			m_pHdr->set_len(m_uiLen);
			m_pHdr->set_type(NORMAL_PKG);
#ifdef __USING_COMPRESSED_DATA__
			if (bCompressed && (m_uiLen > __PKG_COMPRESSED_THRESHOLD__))
			{
				ASSERT(m_queue_compressed.empty());
				ASSERT(nm_zlib::compress(m_queue_mem, m_queue_compressed, -1/*Z_DEFAULT_COMPRESSION*/) >= 0);
				m_queue_mem.clear();
				return m_queue_compressed;
			}
			else
			{
				return m_queue_mem;
			}
#else
			return m_queue_mem;
#endif
		}

	private:
		void body_ready()
		{
			if (!m_bPending)
			{
				return;
			}

			B *pB = (B*) (m_pMem->get_cur_buf());
			m_pMem->inc_len(pB->get_real_size());
			m_pMem->inc_offset(pB->get_real_size());
			m_bPending = false;
		}

	private:
		H *m_pHdr;
		us16_t m_usCnt; //estimate how many package(body).
		ui32_t m_uiLen;
		bool m_bPending; //one package is ready
		mem_ptr_t m_pMem;
		mem_queue_t m_queue_mem;
#ifdef __USING_COMPRESSED_DATA__
		mem_queue_t m_queue_compressed;
#endif
	};
}

#endif /* __ARCHIVE_H__ */
