/*
 * socket.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#include "sn_socket_impl.h"

#if __PLATFORM__ == __PLATFORM_LINUX__
#include <sys/types.h>
#include <sys/socket.h>
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
		CMN_ASSERT(!is_opened());
		if (is_opened())
		{
			close();
		}

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
		bindAddr.sin_port = ::htons(ui16BindPort);
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
		destAddr.sin_port = ::htons(ui16AcceptorPort);
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

	int32_t CTcpSock::sendex(nm_mem::mem_ptr_t &pData)
	{
		CMN_ASSERT(pData->get_cur_len() > 0);

		//if there have been a block of data needed sending, just push me after it.
		if (!m_qSendCache.empty())
		{
			if (!(1 == m_qSendCache.back().m_pData->get_ref_cnt()) && (CMNERR_SUC
					== m_qSendCache.back().m_pData->append(pData->get_data(), pData->get_len())))
			{
				m_qSendCache.push_back(SSendInfo(pData));
			}

			return CMNERR_SEND_PENDING;
		}

		SSendInfo snder(pData);
		int32_t i32Ret = 0;
		for (;;)
		{
			i32Ret = ::send(m_hSock, (const char*) (pData->get_buf() + snder.get_offset()),
					pData->get_len() - snder.get_offset(), 0/*MSG_NOSIGNAL*/);
			if (i32Ret < 0)
			{
#if (__PLATFORM__ == __PLATFORM_LINUX__)
				if (EWOULDBLOCK/*EAGAIN*/== errno)
#elif defined(__PLATEFORM_WINDOWS__)
				if (WSAEWOULDBLOCK/*EAGAIN*/==::GetLastError())
#endif
				{
					//ASSERT(false); //should not reach here
					CMN_ASSERT(pData->get_len() > snder.get_offset());
					m_qSendCache.push_back(snder);//cache it.
					i32Ret = CMNERR_SEND_PENDING;
				}
				else
				{
					//error occurred!
					//TRACE_LAST_ERR( send);
					//close_sock();
					i32Ret = CMNERR_IO_ERR;
				}
				break;
			}

#ifdef __FOR_DEBUG__
			if (0 == i32Ret)
			{
				TRACE_LAST_ERR(send);
				continue;
			}
#endif
			CMN_ASSERT(0 != i32Ret); //what the meaning if zero??

			if (i32Ret < (pData->get_len() - snder.get_offset()))
			{
				snder.set_offset(snder.get_offset() + i32Ret);
				m_qSendCache.push_back(snder);//cache it.
				i32Ret = CMNERR_SEND_PENDING;
				break;
			}

			snder.set_offset(snder.get_offset() + i32Ret);
			if (snder.get_offset() == pData->get_len())
			{
				//TRACE_LOG(LOG, ELL_DEBUG, L"%d is sent\n", ptr->get_cur_len());
				//all are sent.
				i32Ret = CMNERR_SUC;
				break;
			}
		}

		return i32Ret;
	}

	int32_t CTcpSock::send(nm_mem::mem_ptr_t &pData)
	{
		CMN_ASSERT(pData->get_cur_len() > 0);

		//if there have been a block of data needed sending, just push me after it.
		if (!m_qSendCache.empty())
		{
			if (!(1 == m_qSendCache.back().m_pData->get_ref_cnt()) && (CMNERR_SUC
					== m_qSendCache.back().m_pData->append(pData->get_data(), pData->get_len())))
			{
				m_qSendCache.push_back(SSendInfo(pData));
			}

			return CMNERR_SEND_PENDING;
		}

		SSendInfo snder(pData);
		int32_t i32Ret = 0;
		for (;;)
		{
			i32Ret = ::send(m_hSock, (const char*) (pData->get_buf() + snder.get_offset()),
					pData->get_len() - snder.get_offset(), 0/*MSG_NOSIGNAL*/);
			if (i32Ret < 0)
			{
#if (__PLATFORM__ == __PLATFORM_LINUX__)
				if (EWOULDBLOCK/*EAGAIN*/== errno)
#elif defined(__PLATEFORM_WINDOWS__)
				if (WSAEWOULDBLOCK/*EAGAIN*/==::GetLastError())
#endif
				{
					//ASSERT(false); //should not reach here
					CMN_ASSERT(pData->get_len() > snder.get_offset());
					m_qSendCache.push_back(snder);//cache it.
					i32Ret = CMNERR_SEND_PENDING;
				}
				else
				{
					//error occurred!
					//TRACE_LAST_ERR( send);
					//close_sock();
					i32Ret = CMNERR_IO_ERR;
				}
				break;
			}

#ifdef __FOR_DEBUG__
			if (0 == i32Ret)
			{
				TRACE_LAST_ERR(send);
				continue;
			}
#endif
			CMN_ASSERT(0 != i32Ret); //what the meaning if zero??

			if (i32Ret < (pData->get_len() - snder.get_offset()))
			{
				snder.set_offset(snder.get_offset() + i32Ret);
				m_qSendCache.push_back(snder);//cache it.
				i32Ret = CMNERR_SEND_PENDING;
				break;
			}

			snder.set_offset(snder.get_offset() + i32Ret);
			if (snder.get_offset() == pData->get_len())
			{
				//TRACE_LOG(LOG, ELL_DEBUG, L"%d is sent\n", ptr->get_cur_len());
				//all are sent.
				i32Ret = CMNERR_SUC;
				break;
			}
		}

		return i32Ret;
	}

	/**
	 *
	 * */
	int32_t CTcpSock::handle_can_send()
	{
		CScopeLock < CAutoLock > lock(m_lock_send);

		int32_t i32Ret = 0;
		while (!m_queue_send.empty())
		{
			SSnder &snder = m_queue_send.front();
			for (;;)
			{
				i32Ret = ::send(m_iSock, (const char*) (snder.m_ptr->get_buf() + snder.get_offset()),
						snder.m_ptr->get_len() - snder.get_offset(), 0);
				if (i32Ret < 0)
				{
#if defined(__PLATEFORM_LINUX__)
					if (EWOULDBLOCK == errno)
#elif defined(__PLATEFORM_WINDOWS__)
					if (WSAEWOULDBLOCK == ::GetLastError())
#endif
					{
						//ASSERT(false);//should not reach here.
						ASSERT(snder.m_ptr->get_len() > snder.get_offset());
						i32Ret = SEND_IS_PENDING;
					}
					else
					{
						TRACE_LAST_ERR(send);
						//close_sock();
					}
					break;
				}

				ASSERT(i32Ret != 0);

				if (i32Ret < (snder.m_ptr->get_len() - snder.get_offset()))
				{
					snder.set_offset(snder.get_offset() + i32Ret);
					i32Ret = SEND_IS_PENDING;
					break;
				}

				snder.set_offset(snder.get_offset() + i32Ret);
				i32Ret = RET_SUC;
				if (snder.get_offset() == snder.m_ptr->get_len())
				{
					//all are sent.
					m_queue_send.pop_front();
					break;
				}
			}

			if (i32Ret != RET_SUC)
			{
				break;
			}
		}

		return i32Ret;
	}

	int32_t CTcpSock::send(cmn_pvoid_t pV, u_int32_t ui32Len)
	{

	}

	int32_t CTcpSock::recv(nm_mem::mem_ptr_t&)
	{

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
