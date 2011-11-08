#ifndef __BASE_DISPATCH_H__
#define __BASE_DISPATCH_H__

#include <deque>
#if (__USING_C11__)
#include <unordered_map> //should add -std=c++0x with gcc
#elif (__USING_GOOGLE_MAP__)
#include <google/dense_hash_map>
#else
#include <map>
#endif

#include <common/common.h>
#include <memory/mem.h>
#include <utils/singleton.h>

#include "pkg.h"

#ifdef __USING_COMPRESSED_DATA__
#include "zlib_wrapper.h"
#endif

#define DISPATCHER(CONN) \
	nm_utils::CSingleton<nm_pkg::CDispatcher<nm_utils::CSmartPtr<CONN> > >::instance()

#define DISPATCH_PKG(CONN, P_CONN_OBJ, PKG_HDR, PKG_VER) \
	nm_utils::CSmartPtr<CONN> pConn(P_CONN_OBJ); \
	DISPATCHER(CONN).dispatch<PKG_HDR>(pConn, pData, PKG_VER);

#define DISPATCH(CONN) \
	DISPATCH_PKG(CONN, this, nm_pkg::CPkgHdr, PKG_VER)

#define SET_PKG_HANDLER(CONN, PKG) \
	DISPATCHER(CONN).reg_fun((int)PKG::PKG_OPCODE, &nm_pkg::CDispatcher<nm_utils::CSmartPtr<CONN> >::dispatch_fun<PKG>)

namespace nm_pkg
{
	using nm_mem::mem_ptr_t;

	template<typename PCONN>
	class CDispatcher
	{
		typedef void (*P_FUN)(PCONN&, nm_mem::mem_ptr_t&, u_int32_t, u_int32_t);
#if (__USING_C11__)
		typedef STD::unordered_map<int32_t/*msg code*/, P_FUN/*handle function*/> hash_map;
#elif (__USING_GOOGLE_MAP__)
		typedef google::dense_hash_map<int32_t/*msg code*/, P_FUN/*handle function*/> hash_map;
#else

#endif
	public:
		CDispatcher()
		{
#if (__USING_GOOGLE_MAP__)
			m_hmFuns.set_empty_key(-1);
#endif
		}

	public:
		template<typename PKG>
		static void dispatch_fun(PCONN &pConn, mem_ptr_t &pMem, u_int32_t ui32Size/*size of package bodys*/, u_int32_t ui32Tag)
		{
			///
			while (ui32Size > 0)
			{
				///
				PKG *pPkg = (PKG*) (pMem->get_data());
				pMem->dec_head_data(pPkg->get_real_size());
				ui32Size -= pPkg->get_real_size();
				///
				pConn->handle_pkg(*pPkg, ui32Tag);
			}
		}

		void reg_fun(int32_t i32Opcode, P_FUN pFun)
		{
			CMN_ASSERT(m_hmFuns.find(i32Opcode) == m_hmFuns.end());
			m_hmFuns[i32Opcode] = pFun;
		}

		void dispatch(PCONN &pConn, int32_t i32Opcode, mem_ptr_t &pMem, u_int32_t uiSize, u_int32_t uiTag)
		{
			CMN_ASSERT(m_hmFuns.find(i32Opcode) != m_hmFuns.end());
			m_hmFuns[i32Opcode](pConn, pMem, uiSize, uiTag);
		}

		template<typename H>
		void dispatch(PCONN &pConn, mem_ptr_t &pMem, cmn_byte_t cByte)
		{
			int32_t i32Ret = CMNERR_SUC;
			static const u_int32_t s_ui32PkgHdr = sizeof(H);
			while (pMem->get_len() >= s_ui32PkgHdr /*uiBytesDataLen*//*uiPkgHdr*/)
			{
				///
				H *pHdr = (H*) (pMem->get_data());
				if (pHdr->get_ver() != cByte/*VERSION*/
						|| pHdr->get_len() < s_ui32PkgHdr)
				{
					CMN_ASSERT(false);
					i32Ret = CMNERR_COMMON_ERR;
					break;
				}
				///
				if (pHdr->get_len() == s_ui32PkgHdr)
				{
					pMem->dec_head_data(s_ui32PkgHdr);
					continue;
				}
				///
				if (pHdr->get_len() > pMem->get_len())
				{
					pMem->move_data_ahead();
					i32Ret = CMNERR_INCOMPLETED_PKG;
					break;
				}
				///
				if (m_hmFuns.find(pHdr->get_opcode()) == m_hmFuns.end())
				{
					CMN_ASSERT(false);
					i32Ret = CMNERR_UNKNOWN_PKG;
					break;
				}
				///
				pMem->dec_head_data(s_ui32PkgHdr);
				dispatch(pConn, pHdr->get_opcode(), pMem, pHdr->get_len() - s_ui32PkgHdr, pHdr->get_tag());
			}

			if ((CMNERR_COMMON_ERR == i32Ret)
					|| (CMNERR_UNKNOWN_PKG == i32Ret))
			{
				pConn->close();
			}
		}

	private:
		hash_map m_hmFuns;
	};

#if 0
	template<typename PCONN>
	class CUdpPkgDispMgr
	{
		typedef void (*P_FUN)(PCONN&, mem_ptr_t&, u_int32_t, us16_t, u_int32_t);
		typedef STD::unordered_map<int/*msg code*/, P_FUN/*handle function*/> hash_map;
	public:
		template<typename PKG>
		static void dispatch_fun(PCONN &pConn, mem_ptr_t &ptr, u_int32_t uiIp, us16_t usPort, u_int32_t uiTag)
		{
			PKG *pPkg = (PKG*) (ptr->get_cur_buf());
			if (ptr->get_cur_len() != pPkg->get_real_size())
			{
				TRACE_LOG(LOG, ELL_ERR, L"dispatch error!\n");
				//pConn->close();
				return;
			}
			pConn->process(ptr, *pPkg, uiIp, usPort, uiTag);
		}

		void reg_fun(int iKey, P_FUN pFun)
		{
			ASSERT(m_hmFuns.find(iKey) == m_hmFuns.end());
			m_hmFuns[iKey] = pFun;
		}

		//process udp pkg
		template<typename H>
		void dispatch(PCONN &pConn, mem_ptr_t &pMem, u_int32_t uiIp, us16_t usPort, u_int32_t uiVer)
		{
			static const u_int32_t uiPkgHdr = sizeof(H);
			u_int32_t uiSize = pMem->get_cur_len();

			//
			if (uiSize < MINI_PKG_HDR_SIZE/*uiBytesDataLen*/)
			{
				TRACE_LOG(LOG, ELL_ERR, L"package error\n");
				//pConn->close();
				return;
			}
			//
#ifdef __USING_COMPRESSED_DATA__
			if ((*(pMem->get_cur_buf() + PKG_SIZE_TYPE_SIZE)) != COMPRESSED_PKG
					&& (*(pMem->get_cur_buf() + PKG_SIZE_TYPE_SIZE)) != NORMAL_PKG)
#else
			if ((*(pMem->get_tail_free_buf() + PKG_SIZE_TYPE_SIZE)) != NORMAL_PKG)
#endif
			{
				TRACE_LOG(LOG, ELL_ERR, L"fatal error, the pkg is neither a commpressed pkg not a normal pkg!\n");
				//pConn->close();
				return;
			}

#ifdef __USING_COMPRESSED_DATA__
			u_int32_t uiTempDataLen = NTOHL(*((u_int32_t*)(pMem->get_cur_buf())));
			ASSERT(uiSize == uiTempDataLen);

			if ((*(pMem->get_cur_buf() + PKG_SIZE_TYPE_SIZE)) == COMPRESSED_PKG)
			{
				int iLen = 0;
				mem_queue_t tmpQueue;
				mem_queue_t recv_data;
				recv_data.push_back(pMem);
				if ((iLen = nm_zlib::uncompress(recv_data, tmpQueue)) < 0)
				{
					TRACE_LOG(LOG, ELL_ERR, L"uncompress data failed!\n");
					//pConn->close();
					return;
				}
				ASSERT(tmpQueue.size() == 1);
				pMem = tmpQueue.front();
				uiSize = iLen;
			}
#else
			if (uiSize <= uiPkgHdr)
			{
				TRACE_LOG(LOG, ELL_ERR, L"package error\n");
				//pConn->close();
				return;
			}
#endif
			//
			H *pHdr = (H*) (pMem->get_cur_buf());
			if (pHdr->get_ver() != uiVer || pHdr->get_len() < uiPkgHdr)
			{
				TRACE_LOG(LOG, ELL_ERR, L"package error\n");
				//pConn->close();
				return;
			}
			//
			if (uiSize != pHdr->get_len())
			{
				TRACE_LOG(LOG, ELL_ERR, L"package error\n");
				//pConn->close();
				return;
			}
			//
			if (m_hmFuns.find(pHdr->get_opcode()) == m_hmFuns.end())
			{
				TRACE_LOG(LOG, ELL_ERR, L"unknown protocol\n");
				//pConn->close();
				return;
			}
			pMem->inc_offset(uiPkgHdr);
			m_hmFuns[pHdr->get_opcode()](pConn, pMem, uiIp, usPort, pHdr->get_tag());
			//dispatch(pConn, pHdr->get_opcode(), recv_data, pHdr->get_len() - uiPkgHdr, pHdr->get_tag());
		}

	private:
		hash_map m_hmFuns;
	};
#endif

//demo
//class CMsg
//{
//public:
//	CMsg(byte_t *pBytes, u_int32_t ui)
//	{

//	}
//	void process(int i)
//	{
//		cout<<"haha"<<endl;
//	}
//};

//CDispatchManager<int> aaa;
//aaa.add_fun(1, &CDispatchManager<int>::dispatch_fun<CMsg>);

//int i = 0;
//aaa.dispatch(i, 1, NULL, 0);
}

#endif
