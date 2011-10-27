/*
 * socket.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#include "sn_socket_impl.h"

#if (__PLATFORM__ == __PLATFORM_LINUX__)
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h> /* superset of previous */
#endif

#include <utils/utils.h>

namespace nm_network
{
	/**
	 * tcp socket.
	 * */
	CTcpSock::CTcpSock() :
		m_hSock(INVALID_SOCKET)
	{
	}

	CTcpSock::~CTcpSock()
	{
	}

	int32_t CTcpSock::open(sock_handle_t hSock)
	{
		IF_TRUE_THEN_RETURN_CODE(is_opened(), CMNERR_COMMON_ERR);

		if (INVALID_SOCKET < hSock)
		{
			m_hSock = hSock;
		}
		else
		{
			m_hSock = socket(AF_INET, SOCK_STREAM, 0);
		}

		return (INVALID_SOCKET < m_hSock) ? CMNERR_SUC : SNERR_CREAT_SOCK_FAILDED;
	}

	int32_t CTcpSock::close()
	{
		if (INVALID_SOCKET < m_hSock)
		{
			::close(m_hSock);
			m_hSock = INVALID_SOCKET;
		}

		return CMNERR_SUC;
	}

	int32_t CTcpSock::set_nonblock(bool bFlag)
	{
		return nm_utils::set_block_flag(m_hSock, !bFlag);
	}

	int32_t CTcpSock::bind(CIpv4Addr &localAddr)
	{
		CMN_ASSERT(INVALID_SOCKET < m_hSock);

		struct sockaddr_in bindAddr;
		bindAddr.sin_family = AF_INET;
		bindAddr.sin_port = localAddr.get_port_nbo();
		bindAddr.sin_addr = *(static_cast<struct in_addr*> (localAddr.get_ip_nbo()));

		return ::bind(m_hSock, (struct sockaddr*) &bindAddr, sizeof(bindAddr));
	}

	int32_t CTcpSock::bind(const std::string &strBindIP, u_int16_t ui16BindPort)
	{
		CMN_ASSERT(INVALID_SOCKET < m_hSock);

		struct sockaddr_in bindAddr;
		bindAddr.sin_family = AF_INET;
		bindAddr.sin_port = htons(ui16BindPort);
		bindAddr.sin_addr.s_addr = ::inet_addr(strBindIP.c_str());

		return ::bind(m_hSock, (struct sockaddr*) &bindAddr, sizeof(bindAddr));
	}

	int32_t CTcpSock::listen(int32_t i32Backlog)
	{
		return ::listen(m_hSock, i32Backlog);
	}

	tcp_sock_ptr_t CTcpSock::accept()
	{
		tcp_sock_ptr_t pTcpSock = NULL;

		struct sockaddr_in remoteAddr;
		ZERO_MEM(&remoteAddr, sizeof(remoteAddr));
		socklen_t slSize = sizeof(remoteAddr);
		sock_handle_t sockhandle = ::accept(m_hSock, reinterpret_cast<struct sockaddr*> (&remoteAddr), &slSize);
		if (INVALID_SOCKET >= sockhandle)
		{
			CMN_ASSERT(false);
			return pTcpSock;
		}

		pTcpSock = SYS_NOTRW_NEW(CTcpSock);
		CMN_ASSERT(NULL != pTcpSock);
		pTcpSock->open(sockhandle);

		return pTcpSock;
	}

	int32_t CTcpSock::connect(const CIpv4Addr &remoteAddr)
	{
		struct sockaddr_in destAddr;
		destAddr.sin_family = AF_INET;
		destAddr.sin_port = remoteAddr.get_port_nbo();
		destAddr.sin_addr = *static_cast<struct in_addr*> (remoteAddr.get_ip_nbo());

		int32_t i32ret = ::connect(m_hSock, reinterpret_cast<struct sockaddr*> (&destAddr), sizeof(destAddr));

		return CMNERR_SUC == i32ret ? CMNERR_SUC : (EINPROGRESS == errno ? SNERR_IN_PROGRESS : CMNERR_COMMON_ERR);
	}

	int32_t CTcpSock::connect(const cmn_string_t &strAcceptorIp, u_int64_t ui16AcceptorPort)
	{
		struct sockaddr_in destAddr;
		destAddr.sin_family = AF_INET;
		destAddr.sin_port = htons(ui16AcceptorPort);
		destAddr.sin_addr.s_addr = ::inet_addr(strAcceptorIp.c_str());

		int32_t i32ret = ::connect(m_hSock, reinterpret_cast<struct sockaddr*> (&destAddr), sizeof(destAddr));

		return CMNERR_SUC == i32ret ? CMNERR_SUC : (EINPROGRESS == errno ? SNERR_IN_PROGRESS : CMNERR_COMMON_ERR);
	}

	sock_handle_t CTcpSock::get_handle()
	{
		return m_hSock;
	}

	bool CTcpSock::is_opened()
	{
		return (INVALID_SOCKET < m_hSock);
	}

	//	int32_t CTcpSock::sendex(nm_mem::mem_ptr_t &pData)
	//	{
	//		CMN_ASSERT(pData->get_cur_len() > 0);
	//
	//		//if there have been a block of data needed sending, just push me after it.
	//		if (!m_qSendCache.empty())
	//		{
	//			if (!(1 == m_qSendCache.back().m_pData->get_ref_cnt()) && (CMNERR_SUC
	//					== m_qSendCache.back().m_pData->append(pData->get_data(), pData->get_len())))
	//			{
	//				m_qSendCache.push_back(SSendInfo(pData));
	//			}
	//
	//			return CMNERR_SEND_PENDING;
	//		}
	//
	//		SSendInfo snder(pData);
	//		int32_t i32Ret = 0;
	//		for (;;)
	//		{
	//			i32Ret = ::send(m_hSock, (const char*) (pData->get_buf() + snder.get_offset()),
	//					pData->get_len() - snder.get_offset(), 0/*MSG_NOSIGNAL*/);
	//			if (i32Ret < 0)
	//			{
	//#if (__PLATFORM__ == __PLATFORM_LINUX__)
	//				if (EWOULDBLOCK/*EAGAIN*/== errno)
	//#elif defined(__PLATEFORM_WINDOWS__)
	//				if (WSAEWOULDBLOCK/*EAGAIN*/==::GetLastError())
	//#endif
	//				{
	//					//ASSERT(false); //should not reach here
	//					CMN_ASSERT(pData->get_len() > snder.get_offset());
	//					m_qSendCache.push_back(snder);//cache it.
	//					i32Ret = CMNERR_SEND_PENDING;
	//				}
	//				else
	//				{
	//					//error occurred!
	//					//TRACE_LAST_ERR( send);
	//					//close_sock();
	//					i32Ret = CMNERR_IO_ERR;
	//				}
	//				break;
	//			}
	//
	//#ifdef __FOR_DEBUG__
	//			if (0 == i32Ret)
	//			{
	//				TRACE_LAST_ERR(send);
	//				continue;
	//			}
	//#endif
	//			CMN_ASSERT(0 != i32Ret); //what the meaning if zero??
	//
	//			if (i32Ret < (pData->get_len() - snder.get_offset()))
	//			{
	//				snder.set_offset(snder.get_offset() + i32Ret);
	//				m_qSendCache.push_back(snder);//cache it.
	//				i32Ret = CMNERR_SEND_PENDING;
	//				break;
	//			}
	//
	//			snder.set_offset(snder.get_offset() + i32Ret);
	//			if (snder.get_offset() == pData->get_len())
	//			{
	//				//TRACE_LOG(LOG, ELL_DEBUG, L"%d is sent\n", ptr->get_cur_len());
	//				//all are sent.
	//				i32Ret = CMNERR_SUC;
	//				break;
	//			}
	//		}
	//
	//		return i32Ret;
	//	}

	int32_t CTcpSock::send(nm_mem::mem_ptr_t &pData)
	{
		CMN_ASSERT(pData->get_len() > 0);
		CMN_ASSERT(pData->get_ref_cnt() == 1);

		if (!m_lkSending.try_lock()) ///try send lock failed
		{
			///put the msg into queue, then return.
			nm_utils::spin_scopelk_t splk(m_lkSendQ);
			if (m_qSendCache.empty())
			{
				m_qSendCache.push_back(pData);
			}
			else
			{
				if (!(CMNERR_SUC == m_qSendCache.back()->append(pData->get_data(), pData->get_len())))
				{
					m_qSendCache.push_back(pData);
				}
			}

			return CMNERR_SEND_PENDING;
		}
		else
		{
			nm_utils::spin_scopelk_t splk(m_lkSendQ);
			//if there have been a block of data needed sending, just push me after it.
			if (!m_qSendCache.empty())
			{
				m_lkSending.unlock();

				if (CMNERR_SUC != m_qSendCache.back()->append(pData->get_data(), pData->get_len()))
				{
					m_qSendCache.push_back(pData);
				}

				return CMNERR_SEND_PENDING;
			}
		}

		int32_t i32Ret = ::send(m_hSock, (const char*) (pData->get_data()), pData->get_len(), 0/*MSG_NOSIGNAL*/);
		if (i32Ret < 0)
		{
#if (__PLATFORM__ == __PLATFORM_LINUX__)
			if (EWOULDBLOCK/*EAGAIN*/== errno)
#elif defined(__PLATEFORM_WINDOWS__)
			if (WSAEWOULDBLOCK/*EAGAIN*/==::GetLastError())
#endif
			{
				m_lkSending.unlock();
				return CMNERR_SEND_PENDING;
			}
			else
			{
				m_lkSending.unlock();
				return CMNERR_IO_ERR;
			}
		}

#ifdef __FOR_DEBUG__
		if (0 == i32Ret)
		{
			TRACE_LAST_ERR(send);
			continue;
		}
#endif
		CMN_ASSERT(0 != i32Ret); //what the meaning if zero??

		i32Ret = CMNERR_SUC;
		if (pData->get_len() > i32Ret)
		{
			pData->dec_head_data(i32Ret);
			{
				nm_utils::spin_scopelk_t lk(m_lkSendQ);
				m_qSendCache.push_front(pData);
			}
			i32Ret = CMNERR_SEND_PENDING;
		}

		m_lkSending.unlock();

		return i32Ret;
	}

	/**
	 *
	 * */
	int32_t CTcpSock::handle_can_send()
	{
		nm_utils::mtx_scopelk_t lk(m_lkSending);

		if (m_qSending.empty())
		{
			nm_utils::spin_scopelk_t lk(m_lkSendQ);
			m_qSending.swap(m_qSendCache);
		}

		int32_t i32Ret = CMNERR_SUC;
		while (!m_qSending.empty())
		{
			nm_mem::mem_ptr_t &pData = m_qSending.front();

			i32Ret = ::send(m_hSock, (const char*) (pData->get_data()), pData->get_len(), 0);
			if (i32Ret < 0)
			{
#if (__PLATFORM__ == __PLATFORM_LINUX__)
				if (EWOULDBLOCK == errno)
#elif defined(__PLATEFORM_WINDOWS__)
				if (WSAEWOULDBLOCK == ::GetLastError())
#endif
				{
					//ASSERT(false);//should not reach here.
					i32Ret = CMNERR_SEND_PENDING;
				}
				else if (EINTR == errno)
				{
					continue;
				}
				else
				{
					//TRACE_LAST_ERR(send);
					//close_sock();
					i32Ret = CMNERR_IO_ERR;
					break;
				}
			}

			CMN_ASSERT(0 != i32Ret);

			if (i32Ret < pData->get_len())
			{
				pData->dec_head_data(i32Ret);
				i32Ret = CMNERR_SEND_PENDING;
				break;
			}

			CMN_ASSERT(i32Ret == pData->get_len());
			m_qSending.pop_front();

			if (m_qSending.empty())
			{
				nm_utils::spin_scopelk_t lk(m_lkSendQ);
				m_qSending.swap(m_qSendCache);
			}
		}

		return i32Ret;
	}

	int32_t CTcpSock::send(cmn_pvoid_t pV, u_int32_t ui32Len)
	{

	}

	int32_t CTcpSock::handle_can_recv(u_int32_t uiMemSize)
	{
		if (NULL != m_pRecvData)
		{
			m_pRecvData->move_data_ahead();
		}
		else
		{
			m_pRecvData = NEW_MEM();
		}

		int32_t i32Ret = ::recv(m_hSock, (char*) m_pRecvData->get_tail_free_buf(), m_pRecvData->get_tail_free_size(), 0);
		if (0 == i32Ret)
		{
			//normal shutdown
			//TRACE_LOG(LOG, ELL_INFO, L"the peer has performed an orderly shutdown\n");
			return CMNERR_IO_ERR;
		}
		else if (i32Ret < 0)
		{
#if (__PLATFORM__ == __PLATFORM_LINUX__)
			if (EWOULDBLOCK == errno)
#elif defined(__PLATEFORM_WINDOWS__)
			if (WSAEWOULDBLOCK == ::GetLastError())
#endif
			{
				//ASSERT(false); ///should not reach here
				CMN_ASSERT(false);
				return CMNERR_RECV_PENDING;
			}
			else if (EINTR == errno)
			{
				return CMNERR_SUC;
			}
			else
			{
				return CMNERR_IO_ERR;
			}
		}

		m_pRecvData->inc_len(i32Ret);

		return CMNERR_SUC;
	}

	int32_t CTcpSock::recv(cmn_pvoid_t pV, u_int32_t ui32Size)
	{

	}

	CIpv4Addr& CTcpSock::get_peer_addr()
	{
		return m_peeraddr;
	}

	CIpv4Addr& CTcpSock::get_local_addr()
	{
		return m_localaddr;
	}
}
