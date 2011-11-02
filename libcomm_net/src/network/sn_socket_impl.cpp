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
#include <utils/time_info.h>

namespace nm_network
{
	/**
	 * tcp socket.
	 * */
	CRupSock::CRupSock() :
		m_hSock(INVALID_SOCKET)
	{
	}

	CRupSock::~CRupSock()
	{
	}

	int32_t CRupSock::open(sock_handle_t hSock)
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

		return (INVALID_SOCKET < m_hSock) ? CMNERR_SUC : CMNERR_CREAT_SOCK_FAILED;
	}

	int32_t CRupSock::close()
	{
		if (INVALID_SOCKET < m_hSock)
		{
			::close(m_hSock);
			m_hSock = INVALID_SOCKET;
		}

		return CMNERR_SUC;
	}

	int32_t CRupSock::set_nonblock(bool bFlag)
	{
		return nm_utils::set_block_flag(m_hSock, !bFlag);
	}

	int32_t CRupSock::bind(CIpv4Addr &localAddr)
	{
		CMN_ASSERT(INVALID_SOCKET < m_hSock);

		struct sockaddr_in bindAddr;
		bindAddr.sin_family = AF_INET;
		bindAddr.sin_port = localAddr.get_port_nbo();
		bindAddr.sin_addr = *(static_cast<struct in_addr*> (localAddr.get_ip_nbo()));

		return ::bind(m_hSock, (struct sockaddr*) &bindAddr, sizeof(bindAddr));
	}

	int32_t CRupSock::bind(const std::string &strBindIP, u_int16_t ui16BindPort)
	{
		CMN_ASSERT(INVALID_SOCKET < m_hSock);

		struct sockaddr_in bindAddr;
		bindAddr.sin_family = AF_INET;
		bindAddr.sin_port = htons(ui16BindPort);
		bindAddr.sin_addr.s_addr = ::inet_addr(strBindIP.c_str());

		return ::bind(m_hSock, (struct sockaddr*) &bindAddr, sizeof(bindAddr));
	}

	int32_t CRupSock::listen(int32_t i32Backlog)
	{
		return ::listen(m_hSock, i32Backlog);
	}

	rup_sock_ptr_t CRupSock::accept()
	{
		rup_sock_ptr_t pTcpSock = NULL;

		struct sockaddr_in remoteAddr;
		ZERO_MEM(&remoteAddr, sizeof(remoteAddr));
		socklen_t slSize = sizeof(remoteAddr);
		sock_handle_t sockhandle = ::accept(m_hSock, reinterpret_cast<struct sockaddr*> (&remoteAddr), &slSize);
		if (INVALID_SOCKET >= sockhandle)
		{
			CMN_ASSERT(false);
			return pTcpSock;
		}

		pTcpSock = SYS_NOTRW_NEW(CRupSock);
		CMN_ASSERT(NULL != pTcpSock);
		pTcpSock->open(sockhandle);

		return pTcpSock;
	}

	int32_t CRupSock::connect(const CIpv4Addr &remoteAddr)
	{
		struct sockaddr_in destAddr;
		destAddr.sin_family = AF_INET;
		destAddr.sin_port = remoteAddr.get_port_nbo();
		destAddr.sin_addr = *static_cast<struct in_addr*> (remoteAddr.get_ip_nbo());

		int32_t i32ret = ::connect(m_hSock, reinterpret_cast<struct sockaddr*> (&destAddr), sizeof(destAddr));

		return CMNERR_SUC == i32ret ? CMNERR_SUC : (EINPROGRESS == errno ? CMNERR_IN_PROGRESS : CMNERR_COMMON_ERR);
	}

	int32_t CRupSock::connect(const cmn_string_t &strAcceptorIp, u_int64_t ui16AcceptorPort)
	{
		struct sockaddr_in destAddr;
		destAddr.sin_family = AF_INET;
		destAddr.sin_port = htons(ui16AcceptorPort);
		destAddr.sin_addr.s_addr = ::inet_addr(strAcceptorIp.c_str());

		int32_t i32ret = ::connect(m_hSock, reinterpret_cast<struct sockaddr*> (&destAddr), sizeof(destAddr));

		return CMNERR_SUC == i32ret ? CMNERR_SUC : (EINPROGRESS == errno ? CMNERR_IN_PROGRESS : CMNERR_COMMON_ERR);
	}

	sock_handle_t CRupSock::get_handle()
	{
		return m_hSock;
	}

	bool CRupSock::is_opened()
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

	int32_t CRupSock::send(nm_mem::mem_ptr_t &pData)
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

#if (__USING_OLD_IO_METHOD__)
		int32_t i32Ret = ::send(m_hSock, (const char*) (pData->get_data()), pData->get_len(), 0/*MSG_NOSIGNAL*/);
#else
		struct iovec iov;
		iov.iov_base = pData->get_data();
		iov.iov_len = pData->get_len();
		struct msghdr senddata;
		senddata.msg_name = NULL;
		senddata.msg_namelen = 0;
		senddata.msg_iov = &iov;
		senddata.msg_iovlen = 1;
		senddata.msg_control = NULL;
		senddata.msg_controllen = 0;
		senddata.msg_flags = 0;
		int32_t i32Ret = ::sendmsg(m_hSock, &senddata, 0);
#endif
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
	int32_t CRupSock::handle_can_send()
	{
		nm_utils::mtx_scopelk_t lk(m_lkSending);

		if (m_qSending.empty())
		{
			nm_utils::spin_scopelk_t lk(m_lkSendQ);
			m_qSending.swap(m_qSendCache);
		}

		int32_t i32Ret = CMNERR_SUC;
#if (!__USING_OLD_IO_METHOD__)
		struct iovec iov[__MAX_IO_VEC_CNT__];
		struct msghdr senddata;
		senddata.msg_name = NULL;
		senddata.msg_namelen = 0;
		senddata.msg_control = NULL;
		senddata.msg_controllen = 0;
		senddata.msg_flags = 0;
		int32_t i32IoVecCnt = 0;
		int32_t i32TotalLen = 0;
#endif
		while (!m_qSending.empty())
		{
#if (!__USING_OLD_IO_METHOD__)
			i32IoVecCnt = 0;
			i32TotalLen = 0;
			for (mem_queue_t::iterator iter = m_qSending.begin(); iter != m_qSending.end(); ++iter)
			{
				iov[i32IoVecCnt].iov_base = (*iter)->get_data();
				iov[i32IoVecCnt].iov_len = (*iter)->get_len();
				i32TotalLen += iov[i32IoVecCnt].iov_len;
				i32IoVecCnt++;
				if (__MAX_IO_VEC_CNT__ == i32IoVecCnt)
				{
					break;
				}
			}
			senddata.msg_iov = iov;
			senddata.msg_iovlen = i32IoVecCnt;

			i32Ret = ::sendmsg(m_hSock, &senddata, 0);
#else
			nm_mem::mem_ptr_t &pData = m_qSending.front();
			i32Ret = ::send(m_hSock, (const char*) (pData->get_data()), pData->get_len(), 0);
#endif
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
					break;
				}
				else if (EINTR == errno)
				{
					continue;
				}
				else
				{
					i32Ret = CMNERR_IO_ERR;
					break;
				}
			}

			CMN_ASSERT(0 != i32Ret);

#if (!__USING_OLD_IO_METHOD__)
			if (i32Ret < i32TotalLen)
			{
				for (int32_t i = 0; (i < i32IoVecCnt) && (0 != i32Ret); i++)
				{
					if (i32Ret < iov[i].iov_len)
					{
						(*m_qSending.begin())->dec_head_data(i32Ret);
						break;
					}
					else
					{
						i32Ret -= iov[i].iov_len;
						m_qSending.pop_front();
					}
				}

				i32Ret = CMNERR_SEND_PENDING;
				break;
			}

			m_qSending.clear();
#else
			if (i32Ret < pData->get_len())
			{
				pData->dec_head_data(i32Ret);
				i32Ret = CMNERR_SEND_PENDING;
				break;
			}

			CMN_ASSERT(i32Ret == pData->get_len());
			m_qSending.pop_front();
#endif

			if (m_qSending.empty())
			{
				nm_utils::spin_scopelk_t lk(m_lkSendQ);
				m_qSending.swap(m_qSendCache);
			}
		}

		return i32Ret;
	}

	int32_t CRupSock::send(cmn_pvoid_t pV, u_int32_t ui32Len)
	{

	}

	int32_t CRupSock::handle_can_recv(u_int32_t uiMemSize)
	{
		if (NULL != m_pRecvedData)
		{
			m_pRecvedData->move_data_ahead();
		}
		else
		{
			m_pRecvedData = NEW_MEM(uiMemSize);
		}

		int32_t i32Ret = CMNERR_SUC;
		do
		{
#if (__USING_OLD_IO_METHOD__)
			i32Ret = ::recv(m_hSock, (char*) m_pRecvedData->get_tail_free_buf(), m_pRecvedData->get_tail_free_size(), 0);
#else
			struct iovec iov;
			iov.iov_base = m_pRecvedData->get_tail_free_buf();
			iov.iov_len = m_pRecvedData->get_tail_free_size();
			struct msghdr recvdata;
			recvdata.msg_name = NULL;
			recvdata.msg_namelen = 0;
			recvdata.msg_iov = &iov;
			recvdata.msg_iovlen = 1;
			recvdata.msg_control = NULL;
			recvdata.msg_controllen = 0;
			recvdata.msg_flags = 0;
			i32Ret = ::recvmsg(m_hSock, &recvdata, 0);
#endif
			if (0 == i32Ret)
			{
				//normal shutdown
				//TRACE_LOG(LOG, ELL_INFO, L"the peer has performed an orderly shutdown\n");
				i32Ret = CMNERR_IO_ERR;
				break;
			}
			else if (i32Ret < 0)
			{
#if (__PLATFORM__ == __PLATFORM_LINUX__)
				if (EWOULDBLOCK == errno)
#elif defined(__PLATEFORM_WINDOWS__)
				if (WSAEWOULDBLOCK == ::GetLastError())
#endif
				{
					CMN_ASSERT(false); ///should not reach here
					i32Ret = CMNERR_RECV_PENDING;
					break;
				}
				else if (EINTR == errno)
				{
					i32Ret = CMNERR_RECV_PENDING;
					break;
				}
				else
				{
					i32Ret = CMNERR_IO_ERR;
					break;
				}
			}

			m_pRecvedData->inc_len(i32Ret);
			i32Ret = CMNERR_SUC;
		}
		while (false);

		return i32Ret;
	}

	int32_t CRupSock::recv(cmn_pvoid_t pV, u_int32_t ui32Size)
	{

	}

	CIpv4Addr& CRupSock::get_peer_addr()
	{
		return m_peeraddr;
	}

	CIpv4Addr& CRupSock::get_local_addr()
	{
		return m_localaddr;
	}

	//----------------------------------------------------------------------//
	/*--------------------------------------------------------------------*/

	enum EOpcode
	{
		EP_DATA = 0, ///must be 0.
		EP_HB,
		EP_NAK,
		EP_ACK,
		EP_ALL
	///pkgs which receiver handling
	};

#pragma pack(push, 4)
	/**
	 * rmp header
	 * */
	struct SRmpHdr
	{
		u_int32_t ui24Len :24;
		u_int32_t ui8Opcode :8;
		u_int32_t ui8SrcId :8;
		u_int32_t ui24UnUse :24;
	};

	/**
	 * rmp heartbeat pkg
	 * */
	struct SRmpHb
	{
		u_int64_t ui64LatestSeqNo;
	};

	/**
	 *
	 * */
	struct SRmpNak
	{
		u_int64_t ui64Begin;
		u_int64_t ui64End;
		u_int64_t ui64Id;
	};

	/**
	 *
	 * */
	struct SRmpAck
	{
		u_int64_t ui64SeqNo;
		u_int64_t ui64Id;
	};

	/**
	 *
	 * */
	struct SRmpOdata
	{
		u_int64_t ui64SeqNo;
	};

#pragma pack(pop)

	typedef int32_t (CRmpSock::*P_FUN)(sn_sock_addr_t&);
	struct SHander
	{
		int32_t i32Opcode;
		P_FUN fun;
	};

	struct SHander pkg_handlers[EP_ALL] = { { EP_DATA, &CRmpSock::recvep_handle_odata }, { EP_HB, &CRmpSock::recvep_handle_hb }, { EP_NAK, &CRmpSock::sendep_handle_nak }, { EP_ACK,
			&CRmpSock::sendep_handle_ack } };

	/**
	 *
	 * */
	CRmpSock::CRmpSock(int32_t i32Type) :
		m_i32Type(i32Type)
	{
		if (RMP_RECV_SOCK == i32Type)
		{
			m_vecUnvalidPkgs.resize(__UNORDERED_PKGS_CNT__);
		}
		else if (RMP_SEND_SOCK == i32Type)
		{
			m_vecSendWin.resize(__SEND_WIN_SIZE__);
#if (__USING_GOOGLE_MAP__)
			m_mapRecvers.set_empty_key(0);
#endif
		}
		else
		{
			CMN_ASSERT(false);
		}
	}

	CRmpSock::~CRmpSock()
	{
		close();
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

	/**
	 *
	 * */
	int32_t CRmpSock::open(const cmn_string_t &strMulticast, u_int8_t ui8SenderId, u_int32_t ui32AckConfirmCnt)
	{
		ZERO_MEM(&m_addrMulticast, sizeof(m_addrMulticast));
		m_addrMulticast.sin_addr.s_addr = inet_addr(strMulticast.c_str());
		m_ui8SenderId = ui8SenderId;
		m_ui32SendAckCnt = ui32AckConfirmCnt;

		return open(INVALID_SOCKET);
	}

	/**
	 *
	 * */
	int32_t CRmpSock::close()
	{
		IF_TRUE_THEN_RETURN_CODE(!is_opened(), CMNERR_COMMON_ERR);
		sock_handle_t hSock = m_hSock;
		m_hSock = INVALID_SOCKET;
		::close(hSock);

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

	int32_t CRmpSock::get_local_bind_addr(struct sockaddr_in &addr)
	{
		socklen_t len = sizeof(addr);
		return getsockname(m_hSock, (struct sockaddr*) (&addr), &len);
	}

	int32_t CRmpSock::join_multicast_group()
	{
		if (RMP_SEND_SOCK == m_i32Type)
		{
			return CMNERR_COMMON_ERR;
		}

		struct ip_mreqn mreq;
		memset(&mreq, 0, sizeof(mreq));
		mreq.imr_multiaddr.s_addr = m_addrMulticast.sin_addr.s_addr;
		mreq.imr_address.s_addr = INADDR_ANY;
		mreq.imr_ifindex = 0;

		int32_t i32Ret = setsockopt(m_hSock, SOL_IP, IP_ADD_MEMBERSHIP, (const char*) &mreq, sizeof(mreq));
		if (CMNERR_SUC == i32Ret)
		{
			m_epid.ui64Id = 0;
			struct sockaddr_in addr;
			CMN_ASSERT(CMNERR_SUC == get_local_bind_addr(addr));
			ZERO_MEM(&m_epid, sizeof(m_epid));
			m_epid.ui32BindIp = addr.sin_addr.s_addr;
			m_epid.ui16BindPort = addr.sin_port;
			CMN_ASSERT(m_epid.ui64Id > 0);
		}

		return i32Ret;
	}

	int32_t CRmpSock::leave_multicast_group()
	{
		if (RMP_SEND_SOCK == m_i32Type)
		{
			return CMNERR_COMMON_ERR;
		}

		struct ip_mreqn mreq;
		memset(&mreq, 0, sizeof(mreq));
		mreq.imr_multiaddr.s_addr = m_addrMulticast.sin_addr.s_addr;
		mreq.imr_address.s_addr = INADDR_ANY;
		mreq.imr_ifindex = 0;

		return setsockopt(m_hSock, SOL_IP, IP_DROP_MEMBERSHIP, (const char*) &mreq, sizeof(mreq));
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

	/**
	 *
	 * */
	int32_t CRmpSock::send(nm_mem::mem_ptr_t &pMem)
	{
		///
		CMN_ASSERT((sizeof(SRmpHdr) + sizeof(SRmpOdata)) == pMem->get_offset());
		///
		CMN_ASSERT(CMNERR_SUC == pMem->inc_head_data(sizeof(SRmpOdata)));
		SRmpOdata *pOdata = (SRmpOdata*) (pMem->get_data());
		CMN_ASSERT(CMNERR_SUC == pMem->inc_head_data(sizeof(SRmpHdr)));
		SRmpHdr *pHdr = (SRmpHdr*) (pMem->get_data());
		pHdr->ui8Opcode = EP_DATA;
		pHdr->ui24Len = pMem->get_len();
		///added into send buf
		{
			nm_utils::spin_scopelk_t lk(m_lkSenderWin); ///just for handle multi senders.
			if ((0 != m_ui64ValidSendingDataTail) && (m_ui64SendingSeqNo + 1) != m_ui64ValidSendingDataTail)
			{
				if (CMNERR_SUC == m_vecSendWin[m_ui64ValidSendingDataTail - 1]->append(pMem->get_offset_data(pMem->get_init_offset()), pMem->get_len() - pMem->get_init_offset()))
				{
					pMem->reset();
					return CMNERR_SUC;
				}
				else if ((m_ui64ValidSendingDataTail % m_vecSendWin.capacity()) == m_ui64ValidSendingDataHead)
				{
					return CMNERR_NO_SPACE;
				}
			}
			///
			pOdata->ui64SeqNo = m_ui64ValidSendingDataTail;
			m_vecSendWin[pOdata->ui64SeqNo % m_vecSendWin.capacity()] = pMem;
			m_ui64ValidSendingDataTail++;
		}

		return CMNERR_SUC;
	}

	/**
	 * called by rmp sender ep.
	 * called by io send thread
	 * */
	int32_t CRmpSock::handle_can_send()
	{
		///
		int32_t i32Ret = CMNERR_SUC;
		for (;;)
		{
			///is there some data available?
			if (((m_ui64SendingSeqNo + 1) == m_ui64ValidSendingDataTail))
			{
				i32Ret = CMNERR_NO_DATA;
				break;
			}
			///
			CMN_ASSERT(m_ui64SendingSeqNo < m_ui64ValidSendingDataTail);
			///
			i32Ret = udp_send(m_vecSendWin[m_ui64SendingSeqNo + 1], (const struct sockaddr*) (&m_addrMulticast));
			if ((CMNERR_IO_ERR == i32Ret) || (CMNERR_SEND_PENDING == i32Ret))
			{
				break;
			}
			m_ui64SendingSeqNo++;
		}

		return i32Ret;
	}

	/**
	 * 1，如果是发送端的接受线程在调这个函数，则这个函数只会接受到所有接受端的nak和ack消息
	 * 2，如果是接受端的接受线程在调这个函数，则这个函数只会收到发送端的odata和心跳消息（发送端只有在一段时间内没有发送odata消息的时候，才会发送心跳消息）
	 * 3，而只有接受到odata消息的时候才返回数据。
	 * 4, if return CMNERR_ODATA_READY, then recv a odata in pMem...
	 * */
	int32_t CRmpSock::handle_can_recv(u_int32_t ui32MemSz)
	{
		struct sockaddr_in sSrcAddr;
		int32_t i32Ret = CMNERR_SUC;
		///
		for (;;)
		{
			if (NULL == m_pMem)
			{
				m_pMem = NEW_MEM(ui32MemSz);
			}
			else
			{
				CMN_ASSERT(0 == m_pMem->get_len() && 0 == m_pMem->get_offset());
			}

			i32Ret = udp_recv(m_pMem, sSrcAddr);
			if (CMNERR_SUC != i32Ret)
			{
				break;
			}

			if (RMP_RECV_SOCK == m_i32Type)
			{
				i32Ret = recvep_handle_data(sSrcAddr);
			}
			else if (RMP_SEND_SOCK == m_i32Type)
			{
				i32Ret = sendep_handle_data(sSrcAddr);
			}
			else
			{
				CMN_ASSERT(false);
			}

			if (CMNERR_SUC != i32Ret)
			{
				break;
			}
		}

		return i32Ret;
	}

	///
	int32_t CRmpSock::recvep_handle_data(sn_sock_addr_t &sSrcAddr)
	{
		///
		SRmpHdr *pHdr = (SRmpHdr*) (m_pMem->get_data());
		m_pMem->dec_head_data(sizeof(SRmpHdr));
		///只要发送源ID是一致的，源地址不做校验
		IF_TRUE_THEN_RETURN_CODE(((EP_DATA != pHdr->ui8Opcode && EP_HB != pHdr->ui8Opcode) || pHdr->ui8SrcId != m_ui8SenderId), CMNERR_COMMON_ERR);
		if (m_addrSender.sin_addr.s_addr != sSrcAddr.sin_addr.s_addr || m_addrSender.sin_port != sSrcAddr.sin_port)
		{
			m_addrSender.sin_addr.s_addr = sSrcAddr.sin_addr.s_addr;
			m_addrSender.sin_port = sSrcAddr.sin_port;
		}

		return (this->*(pkg_handlers[pHdr->ui8Opcode].fun))(sSrcAddr);
	}

	/**
	 *
	 * i don't care who is receiver....:)...
	 * */
	int32_t CRmpSock::sendep_handle_data(sn_sock_addr_t &sSrcAddr)
	{
		///
		SRmpHdr *pHdr = (SRmpHdr*) (m_pMem->get_data());
		IF_TRUE_THEN_RETURN_CODE((EP_ACK != pHdr->ui8Opcode && EP_NAK != pHdr->ui8Opcode), CMNERR_COMMON_ERR);
		m_pMem->dec_head_data(sizeof(SRmpHdr));
		return (this->*(pkg_handlers[pHdr->ui8Opcode].fun))(sSrcAddr);
	}

	/**
	 * 这块实现的比较恶心，要注意调用这个函数必须循环调用，直到返回CMNERR_NO_DATA为止。
	 * or will ocuurr error......!!!!!!!!!!!!
	 * */
	int32_t CRmpSock::get_next_recved_data(nm_mem::mem_ptr_t &pMem)
	{
		if ((NULL != m_pMem) && (m_pMem->get_len() > 0))
		{
			pMem = m_pMem;
			m_pMem = NULL;
			return CMNERR_SUC;
		}
		///
		if (m_ui64ValidPkgBegin <= m_ui64ValidPkgEnd)
		{
			pMem = m_vecUnvalidPkgs[m_ui64ValidPkgBegin % m_vecUnvalidPkgs.capacity()];
			m_vecUnvalidPkgs[m_ui64ValidPkgBegin % m_vecUnvalidPkgs.capacity()] = NULL;
			m_ui64ValidPkgBegin++;
			return CMNERR_SUC;
		}
		else
		{
			m_ui64ValidPkgBegin = m_ui64ValidPkgEnd = 0;
		}

		set_ack();

		return CMNERR_NO_DATA;
	}

	/**
	 * 这个函数应该在你调用get_next_recved_data反馈CMNERR_NO_DATA之后调用，而且一定要调用。
	 * */
	void CRmpSock::set_ack()
	{
		m_ui64AppConfirmAck = m_ui64LatestRecvedValidSeqNo;
		if ((m_ui64AppConfirmAck - m_ui64AppConfirmAckTmp) >= m_ui32SendAckCnt)
		{
			m_ui64AppConfirmAckTmp = m_ui64AppConfirmAck;
			///send ack
			cmn_byte_t buf[8192];
			SRmpHdr *pHdr = (SRmpHdr*) buf;
			pHdr->ui24Len = sizeof(SRmpAck) + sizeof(SRmpHdr);
			pHdr->ui8Opcode = EP_ACK;
			SRmpAck *pAck = (SRmpAck*) (buf + sizeof(SRmpHdr));
			pAck->ui64SeqNo = m_ui64AppConfirmAckTmp;
			pAck->ui64Id = m_epid.ui64Id;
			CMN_ASSERT(CMNERR_SUC == udp_send(buf, pHdr->ui24Len, (const struct sockaddr*) (&m_addrSender)));
		}
	}

	/**
	 * recv ep function
	 *
	 * */
	int32_t CRmpSock::recvep_handle_odata(sn_sock_addr_t &sSrcAddr)
	{
		int32_t i32Ret = CMNERR_ODATA_READY;
		SRmpOdata *pOdata = (SRmpOdata*) (m_pMem->get_data());
		m_pMem->dec_head_data(sizeof(SRmpOdata));
		if (pOdata->ui64SeqNo == (1 + m_ui64LatestRecvedValidSeqNo))
		{
			///
			m_ui64LatestRecvedValidSeqNo++;
			///activate the wrong ordered data
			//|----------------|latestrecvvaliddata-----|lostdatabegin-----|lostdataend
			if ((m_ui64LatestRecvedValidSeqNo + 1) == m_ui64UnvalidPkgBegin)
			{
				m_ui64ValidPkgBegin = m_ui64ValidPkgEnd = m_ui64UnvalidPkgBegin;

				///find the valid pkg end
				while (m_ui64ValidPkgEnd < m_ui64UnvalidPkgEnd)
				{
					if (NULL == m_vecUnvalidPkgs[(m_ui64ValidPkgEnd + 1) % m_vecUnvalidPkgs.capacity()])
					{
						break;
					}
					m_ui64ValidPkgEnd++;
				}

				if (m_ui64ValidPkgEnd < m_ui64UnvalidPkgEnd)
				{
					m_ui64UnvalidPkgBegin = m_ui64ValidPkgEnd + 1;
					while (m_ui64UnvalidPkgBegin < m_ui64UnvalidPkgEnd)
					{
						if (NULL != m_vecUnvalidPkgs[m_ui64UnvalidPkgBegin % m_vecUnvalidPkgs.capacity()])
						{
							break;
						}
						m_ui64UnvalidPkgBegin++;
					}
				}
				else
				{
					m_ui64UnvalidPkgBegin = m_ui64UnvalidPkgEnd = 0;
				}

				m_ui64LatestRecvedValidSeqNo = m_ui64ValidPkgEnd;
			}
		}
		else if (pOdata->ui64SeqNo <= m_ui64LatestRecvedValidSeqNo)
		{
			i32Ret = CMNERR_SUC;
			//repeated pkg,discard it
			m_pMem->reset();
		}
		else //ui32SeqNo > (m_ui32LatestRecvedSeqNo+1)
		{
			///lost some pkg or wrong ordered.
			///modify begin .end?
			bool bFlag = true;
			if (pOdata->ui64SeqNo < m_ui64UnvalidPkgBegin)
			{
				m_ui64UnvalidPkgBegin = pOdata->ui64SeqNo;
			}
			else if (pOdata->ui64SeqNo > m_ui64UnvalidPkgEnd)
			{
				if ((pOdata->ui64SeqNo - m_ui64LatestRecvedValidSeqNo) > m_vecUnvalidPkgs.capacity())
				{
					///beyond limitation.
					///discard it
					bFlag = false;
					m_pMem->reset();
				}
				else
				{
					m_ui64UnvalidPkgEnd = pOdata->ui64SeqNo;
				}
			}
			///
			if (bFlag)
			{
				CMN_ASSERT(NULL == m_vecUnvalidPkgs[pOdata->ui64SeqNo % m_vecUnvalidPkgs.capacity()]);
				m_vecUnvalidPkgs[pOdata->ui64SeqNo % m_vecUnvalidPkgs.capacity()] = m_pMem;
				m_pMem = NULL;
			}

			i32Ret = CMNERR_SUC;
		}

		return i32Ret;
	}

	/**
	 * handle heart beat from sender.
	 * */
	int32_t CRmpSock::recvep_handle_hb(sn_sock_addr_t &sRecvAddr)
	{
		int32_t i32Ret = CMNERR_SUC;
		SRmpHb *pHb = (SRmpHb*) (m_pMem->get_data());
		if (pHb->ui64LatestSeqNo > m_ui64LatestRecvedValidSeqNo)
		{
			cmn_byte_t buf[8192];
			SRmpHdr *pHdr = (SRmpHdr*) buf;
			pHdr->ui24Len = sizeof(SRmpNak) + sizeof(SRmpHdr);
			pHdr->ui8Opcode = EP_NAK;
			SRmpNak *pNak = (SRmpNak*) (buf + sizeof(SRmpHdr));
			pNak->ui64Begin = m_ui64LatestRecvedValidSeqNo + 1;
			pNak->ui64End = pHb->ui64LatestSeqNo;
			pNak->ui64Id = m_epid.ui64Id;
			i32Ret = udp_send(buf, pHdr->ui24Len, (const struct sockaddr*) (&m_addrSender));
		}
		else
		{
			///do nothing...
		}

		///zero mem.
		m_pMem->reset();

		return i32Ret;
	}

	/**
	 *
	 * */
	int32_t CRmpSock::sendep_handle_nak(sn_sock_addr_t &sRecvAddr)
	{
		SRmpNak *pNak = (SRmpNak*) (m_pMem->get_data());
		m_pMem->dec_head_data(sizeof(SRmpNak));

		m_mapRecvers[pNak->ui64Id].ui64LastUpdateTimeUs = nm_utils::CTimeInfo::get_current_time_us();
		m_mapRecvers[pNak->ui64Id].ui32Naks++;
		m_ui32Naks++;

		if (pNak->ui64Begin < m_ui64ValidSendingDataHead || pNak->ui64End >= m_ui64ValidSendingDataTail)
		{
			///sorry... discard..
			CMN_ASSERT(false); ///this will occurred when a receiver down, and up after later.
			m_pMem->reset();
			return CMNERR_SUC;
		}

		///find the pkg and send again...
		int32_t i32Ret = CMNERR_SUC;
		for (u_int64_t ui64 = pNak->ui64Begin; ui64 <= pNak->ui64End; ui64++)
		{
			do
			{
				i32Ret = udp_send(m_vecSendWin[ui64 % m_vecSendWin.capacity()], (const struct sockaddr*) &m_addrMulticast);
			}
			while (CMNERR_SEND_PENDING == i32Ret);

			if (CMNERR_SUC != i32Ret)
			{
				break;
			}
		}

		return i32Ret;
	}

	/**
	 *
	 * */
	int32_t CRmpSock::sendep_handle_ack(sn_sock_addr_t &sRecvAddr)
	{
		SRmpAck *pAck = (SRmpAck*) m_pMem->get_data();
		m_pMem->dec_head_data(sizeof(SRmpAck));

		u_int64_t ui64CurTimeUs = nm_utils::CTimeInfo::get_current_time_us();
		m_mapRecvers[pAck->ui64Id].ui64LastUpdateTimeUs = ui64CurTimeUs;

		if (pAck->ui64SeqNo < m_ui64ValidSendingDataHead)
		{
			CMN_ASSERT(false);
			return CMNERR_SUC;
		}

		///
		m_mapRecvers[pAck->ui64Id].ui64LastRecvedSeqNo = pAck->ui64SeqNo;
		u_int64_t ui64MiniAckSeqNo = m_mapRecvers.begin()->second.ui64LastRecvedSeqNo;
		for (recver_map_t::iterator iter = m_mapRecvers.begin(); iter != m_mapRecvers.end();)
		{
			if ((ui64CurTimeUs - iter->second.ui64LastUpdateTimeUs) > m_ui64MaxKeepAliveTimeUs)
			{
				m_mapRecvers.erase(iter++);
			}
			else
			{
				ui64MiniAckSeqNo = ui64MiniAckSeqNo > (*iter).second.ui64LastRecvedSeqNo ? (*iter).second.ui64LastRecvedSeqNo : ui64MiniAckSeqNo;
				++iter;
			}
		}

		///
		if (ui64MiniAckSeqNo < m_ui64ValidSendingDataHead)
		{
			return CMNERR_SUC;
		}

		m_ui64ValidSendingDataHead = ui64MiniAckSeqNo + 1;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CRmpSock::udp_send(nm_mem::mem_ptr_t &pMem, const struct sockaddr* pDestAddr)
	{
		int32_t i32Ret = CMNERR_SUC;
		for (;;)
		{
			i32Ret = sendto(m_hSock, pMem->get_data(), pMem->get_len(), 0, pDestAddr, sizeof(struct sockaddr));
			if (i32Ret < 0)
			{
				if (EWOULDBLOCK == errno)
				{
					i32Ret = CMNERR_SEND_PENDING;
					break;
				}
				else if (EINTR == errno)
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

		return i32Ret;
	}

	int32_t CRmpSock::udp_send(cmn_byte_t *pBytes, u_int32_t ui32Bytes, const struct sockaddr *pDestAddr)
	{
		int32_t i32Ret = CMNERR_SUC;
		for (;;)
		{
			i32Ret = sendto(m_hSock, pBytes, ui32Bytes, 0, pDestAddr, sizeof(struct sockaddr));
			if (i32Ret < 0)
			{
				if (EWOULDBLOCK == errno)
				{
					i32Ret = CMNERR_SEND_PENDING;
					break;
				}
				else if (EINTR == errno)
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

		return i32Ret;
	}

	/**
	 *
	 * */
	int32_t CRmpSock::udp_recv(mem_ptr_t &pMem, struct sockaddr_in &sSrcAddr)
	{
		CMN_ASSERT(pMem->get_offset() == 0 && pMem->get_len() == 0);
#if (__USING_OLD_IO_METHOD__)
		int32_t i32Ret = ::recvfrom(m_hSock, (char*) (pMem->get_buf()), pMem->get_size(), 0, (struct sockaddr*) (&remote_addr), (socklen_t*) (&uiAddrSize));
#else
		struct iovec iov[1];
		iov[0].iov_base = pMem->get_buf();
		iov[0].iov_len = pMem->get_size();
		struct msghdr sRecvData;
		sRecvData.msg_iov = iov;
		sRecvData.msg_iovlen = 1;
		sRecvData.msg_control = NULL;
		sRecvData.msg_controllen = 0;
		sRecvData.msg_name = (struct sockaddr*) (&sSrcAddr);
		sRecvData.msg_namelen = sizeof(struct sockaddr);
		sRecvData.msg_flags = 0;
		int32_t i32Ret = ::recvmsg(m_hSock, &sRecvData, 0);
#endif
		if (i32Ret < 0)
		{
#if (__PLATFORM__ == __PLATFORM_LINUX__)
			if (EWOULDBLOCK == errno)
#elif defined(__PLATEFORM_WINDOWS__)
			if (WSAEWOULDBLOCK == ::GetLastError())
#endif
			{
				CMN_ASSERT(false);
				return CMNERR_RECV_PENDING;
			}
			else if (EINTR == errno)
			{
				CMN_ASSERT(false);
				return CMNERR_RECV_PENDING;
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
				return CMNERR_IO_ERR;
			}
		}

		CMN_ASSERT(0 != i32Ret);
		pMem->inc_len(i32Ret);

		return CMNERR_SUC;
	}
}
