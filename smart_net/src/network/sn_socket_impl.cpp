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
		CMN_ASSERT(pData->get_cur_len() > 0);
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
				if (!CMNERR_SUC == m_qSendCache.back()->append(pData->get_data(), pData->get_len()))
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
				//ASSERT(false); //should not reach here
				//CMN_ASSERT(pData->get_len() > snder.get_offset());
				//m_qSendCache.push_back(snder);//cache it.
				return CMNERR_SEND_PENDING;
			}
			else
			{
				//error occurred!
				//TRACE_LAST_ERR( send);
				//close_sock();
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

		i32Ret = CMNERR_SUC;
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
			for (;;)
			{
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
					else
					{
						//TRACE_LAST_ERR(send);
						//close_sock();
						i32Ret = CMNERR_IO_ERR;
					}
					break;
				}

				CMN_ASSERT(0 != i32Ret);

				if (i32Ret < pData->get_len())
				{
					pData->dec_head_data(i32Ret);
					i32Ret = CMNERR_SEND_PENDING;
					break;
				}

				i32Ret = CMNERR_SUC;
				CMN_ASSERT(i32Ret == pData->get_len());
				m_qSending.pop_front();
				break;
			}

			if (i32Ret != CMNERR_SUC)
			{
				break;
			}

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
			else
			{
				//TRACE_LAST_ERR(recv);
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

#if 0
	/*-----------------------------------------------------------------------------*/
	//
	CUdpSock::CUdpSock()
	: CSocket(SOCK_DGRAM)
	{
	}

	CUdpSock::~CUdpSock()
	{

	}

	int CUdpSock::__asyn_send(mem_ptr_t &ptr, ui32_t uiIp/*network byte order*/, us16_t usPort/*network byte order*/)
	{
		//if there have been a block of data needed sending, just push me after it.
		if (!m_queue_send.empty())
		{
			m_queue_send.push_back(SInfo(ptr, uiIp, usPort));
			return SEND_IS_PENDING;
		}

		ASSERT(ptr->get_cur_len() > 0);
		struct sockaddr_in remote_addr =
		{	0};
		remote_addr.sin_family = AF_INET;
		remote_addr.sin_addr.s_addr = uiIp;
		remote_addr.sin_port = usPort;
		static const ui32_t uiAddrSize = sizeof(remote_addr);
		int iRet = ::sendto(m_iSock, (char*)(ptr->get_cur_buf()), ptr->get_cur_len(),
				0, (const struct sockaddr*)(&remote_addr), uiAddrSize);
		if (iRet < 0)
		{
#if defined(__PLATEFORM_LINUX__)
			if (EWOULDBLOCK == errno)
#elif defined(__PLATEFORM_WINDOWS__)
			if (WSAEWOULDBLOCK == ::GetLastError())
#endif
			{
				m_queue_send.push_back(SInfo(ptr, uiIp, usPort));
				return SEND_IS_PENDING;
			}
#if defined(__PLATEFORM_LINUX__)
			else if (EMSGSIZE == errno)
#elif defined(__PLATEFORM_WINDOWS__)
			else if (WSAEMSGSIZE == ::GetLastError())
#endif
			{
				return MSG_SIZE_ERR;
			}
			else
			{
				TRACE_LAST_ERR(sendto);
				return HAVE_ERR;
			}
		}
		ASSERT(iRet != 0);
		ASSERT(iRet == ptr->get_cur_len());

		return RET_SUC;
	}

	int CUdpSock::asyn_send(mem_ptr_t &ptr, ui32_t uiIp, us16_t usPort)
	{
		if (m_iSock < 0)
		{
			return -10000;
		}

		ScopeLock_t lock(m_lock_send);

		return __asyn_send(ptr, uiIp, usPort);
	}

	int CUdpSock::asyn_send(mem_queue_t &mq, ui32_t uiIp, us16_t usPort)
	{
		if (m_iSock < 0)
		{
			return -10000;
		}

		int iRet = 0;
		ScopeLock_t lock(m_lock_send);

		for (mem_queue_t::iterator iter = mq.begin();
				iter != mq.end(); iter++)
		{
			if ((iRet = __asyn_send(*iter, uiIp, usPort)) < 0)
			{
				return -1;
			}
		}

		return iRet;
	}

	int CUdpSock::asyn_send(mem_ptr_t &ptr)
	{
		return asyn_send(ptr, get_peer_addr().get_ne_ip(), get_peer_addr().get_ne_port());
	}

	int CUdpSock::asyn_send(mem_queue_t &mq)
	{
		return asyn_send(mq, get_peer_addr().get_ne_ip(), get_peer_addr().get_ne_port());
	}

	int CUdpSock::handle_can_send()
	{
		int iRet = 0;
		struct sockaddr_in remote_addr =
		{	0};
		remote_addr.sin_family = AF_INET;
		ScopeLock_t lock(m_lock_send);
		while (!m_queue_send.empty())
		{
			SInfo &snder = m_queue_send.front();
			remote_addr.sin_addr.s_addr = snder.get_ip();
			remote_addr.sin_port = snder.get_port();
			static const ui32_t uiAddrSize = sizeof(remote_addr);
			iRet = ::sendto(m_iSock, (char*)(snder.get_mem()->get_cur_buf()), snder.get_mem()->get_cur_len(), 0,
					(const struct sockaddr*)(&remote_addr), uiAddrSize);
			if (iRet < 0)
			{
#if defined(__PLATEFORM_LINUX__)
				if (EWOULDBLOCK == errno)
#elif defined(__PLATEFORM_WINDOWS__)
				if (WSAEWOULDBLOCK == ::GetLastError())
#endif
				{
					return SEND_IS_PENDING;
				}
#if defined(__PLATEFORM_LINUX__)
				else if (EMSGSIZE == errno)
#elif defined(__PLATEFORM_WINDOWS__)
				else if (WSAEMSGSIZE == ::GetLastError())
#endif
				{
					//so bad
					m_queue_send.pop_front();
					continue;
				}
				else
				{
					TRACE_LAST_ERR(sendto);
					return HAVE_ERR;
				}
			}
			ASSERT(0 != iRet);
			ASSERT(snder.get_mem()->get_cur_len() == iRet);
			m_queue_send.pop_front();
		}

		return RET_SUC;
	}

	int CUdpSock::handle_can_recv(ui32_t uiMemSize)
	{
		int iRet = RET_SUC;
		mem_ptr_t ptr = NULL;
		struct sockaddr_in remote_addr =
		{	0};
		ui32_t uiAddrSize = sizeof(remote_addr);
		for (;;)
		{
			if (NULL == ptr)
			{
				ptr = NEW_MEM(uiMemSize);
			}

			iRet = ::recvfrom(m_iSock, (char*)(ptr->get_buf()), ptr->get_free_size(), 0,
					(struct sockaddr*)(&remote_addr), (socklen_t*)(&uiAddrSize));
			if (iRet < 0)
			{
#if defined(__PLATEFORM_LINUX__)
				if (EWOULDBLOCK == errno)
#elif defined(__PLATEFORM_WINDOWS__)
				if (WSAEWOULDBLOCK == ::GetLastError())
#endif
				{
					iRet = RECV_IS_PENDING;
				}
				else
				{
#ifdef __PLATEFORM_WINDOWS__
					///���windowsƽ̨������udp���෢�����ʱ������һ���رգ���һ������10054�Ĵ���
					if (10054 == ::GetLastError())
					{
						continue;
					}
#endif
					TRACE_LAST_ERR(recvfrom);
				}
				break;
			}
			ASSERT(0 != iRet);
			ptr->set_len(iRet);
			//TRACE_LOG(LOG, ELL_DEBUG, L"%u:%hu\n\n", remote_addr.sin_addr.s_addr, remote_addr.sin_port);
			m_queue_recv.push_back(SInfo(ptr, remote_addr.sin_addr.s_addr, remote_addr.sin_port));
			ptr = NULL;
		}

		return iRet;
	}

	void CUdpSock::clear_buf()
	{
		m_queue_recv.clear();

		m_lock_send.lock();
		m_queue_send.clear();
		m_lock_send.unlock();
	}

	CUdpSock::info_queue_t& CUdpSock::get_recv_data()
	{
		return m_queue_recv;
	}
#endif

	/*--------------------------------------------------------------------*/

	enum EOpcode
	{
		EP_DATA = 0, ///must be 0.
		EP_HB,
		EP_ALL
	};

#pragma pack(push, 4)
	/**
	 * rmp header
	 * */
	struct SRmpHdr
	{
		u_int32_t ui32SeqNo;
		u_int32_t ui8Opcode :8;
		u_int32_t i32Len :24;
		cmn_byte_t data[0];
	};

	/**
	 * rmp heartbeat pkg
	 * */
	struct SRmpHb
	{
		u_int32_t ui32LatestSeqNo;
	};

	/**
	 *
	 * */
	struct SRmpNak
	{
		u_int32_t ui32Begin;
		u_int32_t ui32End;
	};

#pragma pack(pop)

	typedef int32_t (CRmpSock::*P_FUN)();
	struct SHander
	{
		int32_t i32Opcode;
		P_FUN fun;
	};

	struct SHander recvep_pkg_handlers[EP_ALL] = { { EP_DATA, &CRmpSock::handle_odata }, { EP_HB, &CRmpSock::handle_hb } };

	/**
	 *
	 * */
	CRmpSock::CRmpSock()
	{

	}

	CRmpSock::~CRmpSock()
	{

	}

	/**
	 *
	 * */
	int32_t CRmpSock::open(sock_handle_t hSock)
	{
		IF_TRUE_THEN_RETURN_CODE(is_opened(), CMNERR_COMMON_ERR);

		if (INVALID_SOCKET < hSock)
		{
			m_hSock = hSock;

			return CMNERR_SUC;
		}

		///create udp sock
		IF_TRUE_THEN_RETURN_CODE((m_hSock = socket(AF_INET, SOCK_DGRAM, 0)) < 0, CMNERR_COMMON_ERR);

		return CMNERR_SUC;
	}

	int32_t CRmpSock::close()
	{
		IF_TRUE_THEN_RETURN_CODE(!is_opened(), CMNERR_COMMON_ERR);
		sock_handle_t hSock = m_hSock;
		m_hSock = INVALID_SOCKET;
		close(hSock);

		return CMNERR_SUC;
	}

	int32_t CRmpSock::bind(const cmn_string_t &strBindIP, u_int16_t ui16BindPort)
	{
		CMN_ASSERT(INVALID_SOCKET < m_hSock);

		struct sockaddr_in bindAddr;
		bindAddr.sin_family = AF_INET;
		bindAddr.sin_port = htons(ui16BindPort);
		bindAddr.sin_addr.s_addr = inet_addr(strBindIP.c_str());

		return ::bind(m_hSock, (struct sockaddr*) &bindAddr, sizeof(bindAddr));
	}

	int32_t CRmpSock::join_multicast_group(const cmn_string_t &strMulticastIp)
	{
		struct ip_mreqn mreq;
		memset(&mreq, 0, sizeof(mreq));
		mreq.imr_multiaddr.s_addr = inet_addr(strMulticastIp.c_str());
		mreq.imr_address.s_addr = INADDR_ANY;
		mreq.imr_ifindex = 0;

		return (setsockopt(m_hSock, SOL_IP, IP_ADD_MEMBERSHIP, (const char*) &mreq, sizeof(mreq)) < 0) ? CMNERR_COMMON_ERR : CMNERR_SUC;
	}

	sock_handle_t CRmpSock::get_handle()
	{
		return m_hSock;
	}

	bool CRmpSock::is_opened()
	{
		return (INVALID_SOCKET < m_hSock);
	}

	int32_t CRmpSock::set_nonblock(bool bFlag)
	{
		return nm_utils::set_block_flag(m_hSock, !bFlag);
	}

	int32_t CRmpSock::send(nm_mem::mem_ptr_t &pM)
	{

	}

	int32_t CRmpSock::handle_can_recv(u_int32_t ui32)
	{
		int32_t i32Ret = RET_SUC;
		struct sockaddr_in remote_addr = { 0 };
		ui32_t uiAddrSize = sizeof(remote_addr);
		for (;;)
		{
			m_pMem = NEW_MEM(ui32);

			i32Ret = ::recvfrom(m_hSock, (char*) (m_pMem->get_buf()), m_pMem->get_total_free_size(), 0, (struct sockaddr*) (&remote_addr),
					(socklen_t*) (&uiAddrSize));
			if (i32Ret < 0)
			{
#if (__PLATFORM__ == __PLATFORM_LINUX__)
				if (EWOULDBLOCK == errno)
#elif defined(__PLATEFORM_WINDOWS__)
				if (WSAEWOULDBLOCK == ::GetLastError())
#endif
				{
					i32Ret = CMNERR_RECV_PENDING;
				}
				else
				{
#if (__PLATFORM__ == __PLATFORM_WINDOWS__)
					///���windowsƽ̨������udp���෢�����ʱ������һ���رգ���һ������10054�Ĵ���
					if (10054 == ::GetLastError())
					{
						continue;
					}
#endif
					i32Ret = CMNERR_IO_ERR;
				}
				break;
			}
			CMN_ASSERT(0 != i32Ret);
			m_pMem->inc_len(i32Ret);

			if (m_addrSender.sin_addr.s_addr != remote_addr.sin_addr.s_addr)
			{
				m_addrSender.sin_addr.s_addr = remote_addr.sin_addr.s_addr;
			}
			if (m_addrSender.sin_port != remote_addr.sin_port)
			{
				m_addrSender.sin_port = remote_addr.sin_port;
			}

			if (E_RECV_ENDPOINT == m_i32Type)
			{
				handle_recvep_data();
			}
			else if (E_SEND_ENDPOINT == m_i32Type)
			{
				handle_sendep_data();
			}
			else
			{
				CMN_ASSERT(false);
			}
		}
	}

	///
	int32_t CRmpSock::handle_recvep_data()
	{
		///
		SRmpHdr *pHdr = static_cast<SRmpHdr*> (m_pMem->get_data());
		IF_TRUE_THEN_RETURN_CODE((pHdr->ui8Opcode >= EP_ALL), CMNERR_COMMON_ERR);
		m_pMem->dec_head_data(sizeof(SRmpHdr));
		return recvep_pkg_handlers[pHdr->ui8Opcode]();
	}

	///
	int32_t CRmpSock::handle_odata()
	{
		return CMNERR_SUC;
	}

	int32_t CRmpSock::udp_send()
	{
		//		remote_addr.sin_addr.s_addr = snder.get_ip();
		//		remote_addr.sin_port = snder.get_port();
		//		static const ui32_t uiAddrSize = sizeof(remote_addr);
		//		SRmpNak nak;
		//		nake.ui32Begin = m_ui32LatestRecvedSeqNo + 1;
		//nake	.ui32End =
		//	iRet = ::sendto(m_hSock, , , 0,
		//			(const struct sockaddr*)(&remote_addr), uiAddrSize);
	}

	int32_t CRmpSock::handle_hb()
	{
		int32_t i32Ret = CMNERR_SUC;
		SRmpHb *pHb = static_cast<SRmpHb*> (m_pMem->get_data());
		if (pHb->ui32LatestSeqNo > m_ui32LatestRecvedSeqNo)
		{
			SRmpNak nak;
			nake.ui32Begin = m_ui32LatestRecvedSeqNo + 1;
			nake.ui32End = pHb->ui32LatestSeqNo;
			for (;;)
			{
				i32Ret = sendto(m_hSock, &nak, sizeof(nake), 0, (const struct sockaddr*) (&m_addrSender), sizeof(m_addrSender));
				if (i32Ret < 0)
				{
					if (EWOULDBLOCK == errno)
					{
						continue;
					}
					else
					{
						i32Ret = CMNERR_IO_ERR;
						break;
					}
				}
				i32Ret = CMNERR_SUC;
				break;
			}
		}
		else
		{
			///do nothing...
		}

		return i32Ret;
	}

	///
	int32_t CRmpSock::handle_sendep_data()
	{

	}

	int32_t CRmpSock::handle_can_send()
	{
	}

//
//		int32_t bind(const cmn_string_t &strBindIP, u_int16_t ui16BindPort);
//		int32_t join_multicast_group();
//		sock_handle_t get_handle();
//		bool is_opened();
//		int32_t set_nonblock(bool bFlag);
//
//		int32_t send(nm_mem::mem_ptr_t&) = 0;
//		int32_t send(cmn_pvoid_t pV, u_int32_t ui32Len) = 0;
//
//		int32_t handle_can_recv(u_int32_t);

}
