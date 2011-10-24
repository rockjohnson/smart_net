#ifndef __BASE_DISPATCH_H__
#define __BASE_DISPATCH_H__

#include <deque>
#if (__USING_C11__)
#include <unordered_map> //should add -std=c++0x with gcc
#else
#include <google/dense_hash_map>
#endif

#include <common/common.h>
#include <memory/mem.h>
#include "pkg.h"

#ifdef __USING_COMPRESSED_DATA__
#include "zlib_wrapper.h"
#endif

namespace nm_pkg
{
#define DISPATCHER(CONN) \
	CSingleton<nm_pkg::CDispMgr<nm_utils::CSmartPtr<CONN> > >::instance()

#define DISPATCH_PKG(CONN, P_CONN_OBJ, PKG_HDR, PKG_VER) \
	ui32_t uiSize = m_sock.get_recv_data_len(); \
	nm_utils::CSmartPtr<CONN> pConn(P_CONN_OBJ); \
	DISPATCHER(CONN).dispatch<PKG_HDR>(pConn, recv_data, uiSize, PKG_VER);

#define DISPATCH(CONN) \
	DISPATCH_PKG(CONN, this, CPkgHdr, VERSION)
	/*ui32_t uiSize = m_sock.get_recv_data_len(); \
	nm_utils::CSmartPtr<CONN> pConn(this); \
	DISPATCHER(CONN).dispatch<CPkgHdr>(pConn, recv_data, uiSize, VERSION);*/

#define DISPATCHER_UDP_PKG(CONN) \
	CSingleton<nm_pkg::CUdpPkgDispMgr<nm_utils::CSmartPtr<CONN> > >::instance()

#define DISPATCH_UDP_PKG(CONN, PKG_HDR) \
	nm_utils::CSmartPtr<CONN> pConn(this); \
	DISPATCHER_UDP_PKG(CONN).dispatch<PKG_HDR>(pConn, pMem, ulIp, usPort, VERSION);

#define SET_PKG_HANDLER(CONN, PKG) \
	DISPATCHER(CONN).reg_fun((int)PKG::PKG_OPCODE, &CDispMgr<nm_utils::CSmartPtr<CONN> >::dispatch_fun<PKG>)

#define SET_UDP_PKG_HANDLER(CONN, PKG) \
	DISPATCHER_UDP_PKG(CONN).reg_fun((int)PKG::PKG_OPCODE, &CUdpPkgDispMgr<nm_utils::CSmartPtr<CONN> >::dispatch_fun<PKG>)

	using namespace nm_utils;
	using namespace std;

	template<typename PCONN>
	class CDispMgr
	{
		typedef void (*P_FUN)(PCONN&, nm_mem::mem_ptr_t&, u_int32_t, u_int32_t);
#if (__USING_C11__)
		typedef STD::unordered_map<int/*msg code*/, P_FUN/*handle function*/> hash_map;
#else
		typedef google::dense_hash_map<int/*msg code*/, P_FUN/*handle function*/> hash_map;
#endif

	public:
		template<typename PKG>
		static void dispatch_fun(PCONN &pConn, mem_queue_t &raw_data, ui32_t uiSize/*size of package bodys*/, ui32_t uiTag)
		{
#ifdef __FOR_DEBUG__
				ui32_t ui = 0;
				for (mem_queue_t::iterator iter = raw_data.begin();
				iter != raw_data.end(); iter++)
				{
					ui += (*iter)->get_cur_len();
				}
				ASSERT(ui >= uiSize);
#endif

			//PKG pkg(pBytes, uiSize);
			ASSERT(!raw_data.empty());
			static const ui32_t uiPkgSize = sizeof(PKG);
			mem_ptr_t pMem;
			while (uiSize > 0)
			{
				if (NULL == pMem)
				{
					ASSERT(!raw_data.empty());
					pMem = raw_data.front();
					ASSERT(pMem->get_cur_len() > 0);
				}

				//whether the package body is in one memory or not
				if (pMem->get_cur_len() < uiPkgSize)
				{
					//not in one memory
					raw_data.pop_front();
					///�ٴ�ǿ��������Ϣϵͳ��֧�������Ϣ��Ĵ�С����С��MAX_MEM_SIZE
					///ԭ������ڱ��δ��루����ϸ����������Ļ��Ͳ���ʹ���ڴ���е��ڴ����������Ϣ����
					ASSERT((pMem->get_cur_len() + raw_data.front()->get_cur_len()) >= uiPkgSize);//the size of one package body must <= MAX_MEM_SIZE
					//copy data
					ui32_t uiTemp = uiPkgSize - pMem->get_cur_len();
					pMem->rearrange();
					pMem->append(raw_data.front()->get_cur_buf(), uiTemp);
					raw_data.front()->inc_offset(uiTemp);
					//
					if (raw_data.front()->get_cur_len() == 0)
					{
						raw_data.pop_front();
					}
					raw_data.push_front(pMem);
				}

				PKG *pPkg = (PKG*)(pMem->get_cur_buf());
				if (pMem->get_cur_len() < pPkg->get_real_size())
				{
					raw_data.pop_front();
					ASSERT((pMem->get_cur_len() + raw_data.front()->get_cur_len()) >= pPkg->get_real_size()); //the size of one package body must <= MAX_MEM_SIZE
					//
					ui32_t uiTemp = pPkg->get_real_size() - pMem->get_cur_len();
					pMem->rearrange();
					pMem->append(raw_data.front()->get_cur_buf(), uiTemp);
					raw_data.front()->inc_offset(uiTemp);
					//
					if (raw_data.front()->get_cur_len() == 0)
					{
						raw_data.pop_front();
					}
					raw_data.push_front(pMem);
					pPkg = (PKG*)(pMem->get_cur_buf());
				}
				//
				TRACE_LOG(LOG, ELL_DEBUG, L"uiSize %u, real size: %u\n", uiSize, pPkg->get_real_size());
				ASSERT(uiSize >= pPkg->get_real_size());
				uiSize -= pPkg->get_real_size();//uiPkgSize; //pay attention, the size of pkg(not include CPkgHdr) have to be constant.
				pMem->inc_offset(pPkg->get_real_size()/*uiPkgSize*/);
				pConn->process(*pPkg, uiTag);
				//
				if (pMem->get_cur_len() == 0)
				{
					raw_data.pop_front();
					pMem = NULL;
				}
			}
		}

		void reg_fun(int iKey, P_FUN pFun)
		{
			ASSERT(m_hmFuns.find(iKey) ==  m_hmFuns.end());
			m_hmFuns[iKey] = pFun;
		}

		void dispatch(PCONN &pConn, int iCmd, deque<mem_ptr_t> &raw_data, ui32_t uiSize, ui32_t uiTag)
		{
			ASSERT(m_hmFuns.find(iCmd) != m_hmFuns.end());
			m_hmFuns[iCmd](pConn, raw_data, uiSize, uiTag);
		}

		template <typename H>
		void dispatch(PCONN &pConn, mem_queue_t &recv_data, ui32_t uiSize, ui32_t uiVer)
		{
			ASSERT(!recv_data.empty());
			static const ui32_t uiPkgHdr = sizeof(H);
			//static const ui32_t uiBytesDataLen = sizeof(ui32_t);
			mem_ptr_t pMem;
			while (uiSize > MINI_PKG_HDR_SIZE /*uiBytesDataLen*/ /*uiPkgHdr*/)
			{
				if (NULL == pMem)
				{
					ASSERT(!recv_data.empty());
					pMem = recv_data.front();
					ASSERT(pMem->get_cur_len() > 0);
				}
				//
				if (pMem->get_cur_len() < MINI_PKG_HDR_SIZE/*uiBytesDataLen*/)
				{
					//confirm have enough bytes
					pMem->rearrange();
					recv_data.pop_front();
					ui32_t uiTemp = MINI_PKG_HDR_SIZE/*uiBytesDataLen*/ - pMem->get_cur_len();
					pMem->append(recv_data.front()->get_cur_buf(), uiTemp);
					recv_data.front()->inc_offset(uiTemp);
					if (recv_data.front()->get_cur_len() == 0)
					{
						recv_data.pop_front();
					}
					recv_data.push_front(pMem);
				}
				//
#ifdef __USING_COMPRESSED_DATA__
				if ((*(pMem->get_cur_buf() + PKG_SIZE_TYPE_SIZE)) != COMPRESSED_PKG
						&& (*(pMem->get_cur_buf() + PKG_SIZE_TYPE_SIZE)) != NORMAL_PKG)
#else
				if ((*(pMem->get_cur_buf() + PKG_SIZE_TYPE_SIZE)) != NORMAL_PKG)
#endif
				{
					TRACE_LOG(LOG, ELL_ERR, L"fatal error, the pkg is neither a commpressed pkg not a normal pkg!\n");
					pConn->close();
					break;
				}
				//
				//获取此段数据的长度（也许是压缩的、也许是非压缩的）
				ui32_t uiTempDataLen = NTOHL(*((ui32_t*)(pMem->get_cur_buf())));
				//if (uiPkgHdr > uiTempDataLen)  //you see, the data length value must be the first member of pkg header.
				//{
				//	//error
				//	TRACE_LOG(LOG, ELL_ERR, L"fatal error, the length of package is smaller than uiPkgHdr\n");
				//	//pConn->handle_invalid_pkg();
				//	pConn->close();
				//	break;
				//}
				//
				if (uiTempDataLen/*uiPkgHdr*/ > uiSize)  //you see, the data length value must be the first member of pkg header.
				{
					TRACE_LOG(LOG, ELL_DEBUG, L"received data size is smaller than pkg header, pkg size is : %u\n", NTOHL(*((ui32_t*)(pMem->get_cur_buf()))));
					break;
				}

				int iLen = uiTempDataLen;
#ifdef __USING_COMPRESSED_DATA__
				mem_queue_t tmpQueue;
				//�ж��Ƿ���ѹ�������
				if ((*(pMem->get_cur_buf() + PKG_SIZE_TYPE_SIZE)) == COMPRESSED_PKG)
				{
					if ((iLen = nm_zlib::uncompress(recv_data, tmpQueue)) < 0)
					{
						TRACE_LOG(LOG, ELL_ERR, L"uncompress data failed!\n");
						pConn->close();
						break;
					}
					pMem = tmpQueue.front();
				}
#endif
				//
				if (iLen <= uiPkgHdr)
				{
					TRACE_LOG(LOG, ELL_ERR, L"fatal error, the length of pkg(%d) is smaller than or equal to header!\n", iLen);
					pConn->close();
					break;
				}
				//
				if (pMem->get_cur_len() < uiPkgHdr)
				{
					pMem->rearrange();
					recv_data.pop_front();
					ui32_t uiTemp = uiPkgHdr - pMem->get_cur_len();
					pMem->append(recv_data.front()->get_cur_buf(), uiTemp);
					recv_data.front()->inc_offset(uiTemp);
					if (recv_data.front()->get_cur_len() == 0)
					{
						recv_data.pop_front();
					}
					recv_data.push_front(pMem);
				}
				//
				H *pHdr = (H*) (pMem->get_cur_buf());
				if (pHdr->get_ver() != uiVer/*VERSION*/
						//|| pHdr->get_chkcd() != CHECK_CODE
						|| pHdr->get_len() < uiPkgHdr)
				{
					TRACE_LOG(LOG, ELL_ERR, L"package error\n");
					//pConn->handle_invalid_pkg();
					pConn->close();
					break;
				}
				//
				/*if (uiSize < pHdr->get_len())
				{
					TRACE_LOG(LOG, ELL_DEBUG, L"received data size too small, pkg size is : %u\n", ntohl(*((ui32_t*)(pMem->get_cur_buf()))));
					break;
				}*/
				//
				if (m_hmFuns.find(pHdr->get_opcode()) == m_hmFuns.end())
				{
					TRACE_LOG(LOG, ELL_ERR, L"unknown protocol\n");
					//pConn->handle_unknown_pkg();
					pConn->close();
					break;
				}
				//if (pHdr->get_len() <= uiPkgHdr)
				//{
				//	TRACE_LOG(LOG, ELL_ERR, L"pHdr->get_len() <= uiPkgHdr\n");
				//	pConn->close();
				//	break;
				//}
#ifdef __USING_COMPRESSED_DATA__
				uiSize -= uiTempDataLen;
#else
				uiSize -= pHdr->get_len();
#endif
				pMem->inc_offset(uiPkgHdr);
				if (pMem->get_cur_len() == 0)
				{
					recv_data.pop_front();
				}
#ifdef __USING_COMPRESSED_DATA__
				dispatch(pConn, pHdr->get_opcode(), tmpQueue.empty() ? recv_data : tmpQueue, pHdr->get_len() - uiPkgHdr, pHdr->get_tag());
#else
				dispatch(pConn, pHdr->get_opcode(), recv_data, pHdr->get_len()-uiPkgHdr, pHdr->get_tag());
#endif
				pMem = NULL;
			}
		}

		///which will receive the pkg head firstly.
		template <typename H>
		void dispatch_ex(PCONN &pConn, mem_queue_t &recv_data, ui32_t uiSize, ui32_t uiVer)
		{
			ASSERT(!recv_data.empty());
			static const ui32_t uiPkgHdr = sizeof(H);
			//static const ui32_t uiBytesDataLen = sizeof(ui32_t);
			mem_ptr_t pMem;
			while (uiSize > uiPkgHdr)
			{
				if (NULL == pMem)
				{
					ASSERT(!recv_data.empty());
					pMem = recv_data.front();
					ASSERT(pMem->get_cur_len() > 0);
				}
				
				//
				if (pMem->get_cur_len() < uiPkgHdr)
				{
					pMem->rearrange();
					recv_data.pop_front();
					ui32_t uiTemp = uiPkgHdr - pMem->get_cur_len();
					pMem->append(recv_data.front()->get_cur_buf(), uiTemp);
					recv_data.front()->inc_offset(uiTemp);
					if (recv_data.front()->get_cur_len() == 0)
					{
						recv_data.pop_front();
					}
					recv_data.push_front(pMem);
				}
				//
				H *pHdr = (H*) (pMem->get_cur_buf());
				if (pHdr->get_ver() != uiVer
					//|| pHdr->get_chkcd() != CHECK_CODE
					|| pHdr->get_len() < uiPkgHdr)
				{
					TRACE_LOG(LOG, ELL_ERR, L"package error\n");
					//pConn->handle_invalid_pkg();
					pConn->close();
					break;
				}
				//
				if (uiSize < pHdr->get_len())
				{
					TRACE_LOG(LOG, ELL_DEBUG, L"received data size too small, pkg size is : %u\n", NTOHL(*((ui32_t*)(pMem->get_cur_buf()))));
					break;
				}
				//
				if (m_hmFuns.find(pHdr->get_opcode()) == m_hmFuns.end())
				{
					TRACE_LOG(LOG, ELL_ERR, L"unknown protocol, %u\n", pHdr->get_opcode());
					//pConn->handle_unknown_pkg();
					pConn->close();
					break;
				}
				//if (pHdr->get_len() <= uiPkgHdr)
				//{
				//	TRACE_LOG(LOG, ELL_ERR, L"pHdr->get_len() <= uiPkgHdr\n");
				//	pConn->close();
				//	break;
				//}
				uiSize -= pHdr->get_len();
				pMem->inc_offset(uiPkgHdr);
				if (pMem->get_cur_len() == 0)
				{
					recv_data.pop_front();
				}
				dispatch(pConn, pHdr->get_opcode(), recv_data, pHdr->get_len()-uiPkgHdr, pHdr->get_tag());
				pMem = NULL;
			}
		}

	private:
		hash_map m_hmFuns;
	};

	template<typename PCONN>
	class CUdpPkgDispMgr
	{
		typedef void (*P_FUN)(PCONN&, mem_ptr_t&, ui32_t, us16_t, ui32_t);
		typedef STD::unordered_map<int/*msg code*/, P_FUN/*handle function*/>
				hash_map;
	public:
		template<typename PKG>
		static void dispatch_fun(PCONN &pConn, mem_ptr_t &ptr, ui32_t uiIp,
				us16_t usPort, ui32_t uiTag)
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
			ASSERT(m_hmFuns.find(iKey) ==  m_hmFuns.end());
			m_hmFuns[iKey] = pFun;
		}

		//process udp pkg
		template<typename H>
		void dispatch(PCONN &pConn, mem_ptr_t &pMem, ui32_t uiIp, us16_t usPort,
				ui32_t uiVer)
		{
			static const ui32_t uiPkgHdr = sizeof(H);
			ui32_t uiSize = pMem->get_cur_len();

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
			if ((*(pMem->get_cur_buf() + PKG_SIZE_TYPE_SIZE)) != NORMAL_PKG)
#endif
			{
				TRACE_LOG(LOG, ELL_ERR, L"fatal error, the pkg is neither a commpressed pkg not a normal pkg!\n");
				//pConn->close();
				return;
			}

#ifdef __USING_COMPRESSED_DATA__
			ui32_t uiTempDataLen = NTOHL(*((ui32_t*)(pMem->get_cur_buf())));
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
			m_hmFuns[pHdr->get_opcode()](pConn, pMem, uiIp, usPort,
					pHdr->get_tag());
			//dispatch(pConn, pHdr->get_opcode(), recv_data, pHdr->get_len() - uiPkgHdr, pHdr->get_tag());
		}

	private:
		hash_map m_hmFuns;
	};

//demo
//class CMsg
//{
//public:
//	CMsg(byte_t *pBytes, ui32_t ui)
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
