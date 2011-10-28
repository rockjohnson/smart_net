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
	int32_t CRupSock::handle_can_send()
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

	int32_t CRupSock::send(cmn_pvoid_t pV, u_int32_t ui32Len)
	{

	}

	int32_t CRupSock::handle_can_recv(u_int32_t uiMemSize)
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
		SRmpHdr hdr;
		u_int32_t ui32Begin;
		u_int32_t ui32End;
		u_int64_t ui64Id;
	};

	/**
	 *
	 * */
	struct SRmpOdata
	{
		u_int32_t ui32SeqNo;
	};

#pragma pack(pop)

	typedef int32_t (CRmpSock::*P_FUN)();
	struct SHander
	{
		int32_t i32Opcode;
		P_FUN fun;
	};

	struct SHander pkg_handlers[EP_ALL] = { { EP_DATA, &CRmpSock::handle_odata }, { EP_HB, &CRmpSock::handle_hb }, { EP_NAK, &CRmpSock::handle_nak },
			{ EP_ACK, &CRmpSock::handle_ack } };

	/**
	 *
	 * */
#define __UNORDERED_PKGS_CNT__ (100000)
#define __SEND_WIN_SIZE__ (1000000)
	CRmpSock::CRmpSock(int32_t i32EpType)
	{
		if (ERMP_RECV_SOCK == i32EpType)
		{
			m_vecUnorderedPkgs.resize(__UNORDERED_PKGS_CNT__);
		}
		else if (ERMP_SEND_SOCK == i32EpType)
		{
			m_vecSendWin.resize(__SEND_WIN_SIZE__);
		}
		else
		{
			CMN_ASSERT(false);
		}
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

	/**
	 *
	 * */
	int32_t CRmpSock::open(const cmn_string_t &strMulticast)
	{
		ZERO_MEM(&m_addrMulticast, sizeof(m_addrMulticast));
		m_addrMulticast.sin_addr.s_addr = inet_addr(strMulticast.c_str());

		return open();
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

	int32_t CRmpSock::join_multicast_group(const cmn_string_t &strMulticastIp)
	{
		struct ip_mreqn mreq;
		memset(&mreq, 0, sizeof(mreq));
		mreq.imr_multiaddr.s_addr = inet_addr(strMulticastIp.c_str());
		mreq.imr_address.s_addr = INADDR_ANY;
		mreq.imr_ifindex = 0;

		int32_t i32Ret = setsockopt(m_hSock, SOL_IP, IP_ADD_MEMBERSHIP, (const char*) &mreq, sizeof(mreq));
		if (CMNERR_SUC == i32Ret)
		{
			m_epid.ui64Id = 0;
			struct sockaddr_in addr;
			CMN_ASSERT(CMNERR_SUC == get_local_bind_addr(addr));
			m_epid.ui32BindIp = addr.sin_addr.s_addr;
			m_epid.ui16BindPort = addr.sin_port;
			CMN_ASSERT(m_epid.ui64Id > 0);
		}

		return i32Ret;
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
		///
		CMN_ASSERT(CMNERR_SUC == pM->inc_head_data(sizeof(SRmpOdata)));
		SRmpOdata *pOdata = (SRmpOdata*) (pM->get_data());
		///
		CMN_ASSERT(CMNERR_SUC == pM->inc_head_data(sizeof(SRmpHdr)));
		SRmpHdr *pHdr = (SRmpHdr*) (pM->get_data());
		pHdr->ui8Opcode = EP_DATA;
		pHdr->ui24Len = pM->get_len();
		///added into send buf
		{
			nm_utils::spin_scopelk_t lk(m_lkSenderWin); ///just for handle multi senders.
			if ((0 != m_ui32ValidSendingDataTail) && (m_ui32SendingSeqNo + 1) != m_ui32ValidSendingDataTail)
			{
				if (CMNERR_SUC == m_vecSendWin[m_ui32ValidSendingDataTail - 1].m_pMem->append(pM->get_offset_data(pM->get_init_offset()),
						pM->get_len() - pM->get_init_offset()))
				{
					return CMNERR_SUC;
				}
				else if ((m_ui32ValidSendingDataTail % m_vecSendWin.capacity()) == m_ui32ValidSendingDataHead)
				{
					return CMNERR_NO_SPACE;
				}
			}
			///
			pOdata->ui32SeqNo = m_ui32ValidSendingDataTail;
			m_vecSendWin[pOdata->ui32SeqNo % m_vecSendWin.capacity()].m_pMem = pM;
			m_ui32ValidSendingDataTail++;
		}

		return CMNERR_SUC;
	}

	int32_t CRmpSock::handle_can_recv(u_int32_t ui32)
	{
		int32_t i32Ret = CMNERR_SUC;
		struct sockaddr_in remote_addr = { 0 };
		u_int32_t uiAddrSize = sizeof(remote_addr);
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

			if (ERMP_RECV_SOCK == m_i32Type)
			{
				return handle_recvep_data();
			}
			else if (ERMP_SEND_SOCK == m_i32Type)
			{
				return handle_sendep_data();
			}
			else
			{
				CMN_ASSERT(false);
			}
		}

		return CMNERR_COMMON_ERR;
	}

	///
	int32_t CRmpSock::handle_recvep_data()
	{
		///
		SRmpHdr *pHdr = (SRmpHdr*) (m_pMem->get_data());
		IF_TRUE_THEN_RETURN_CODE((pHdr->ui8Opcode >= EP_ALL), CMNERR_COMMON_ERR);
		m_pMem->dec_head_data(sizeof(SRmpHdr));
		return (this->*(pkg_handlers[pHdr->ui8Opcode].fun))();
	}

	/**
	 * 这块实现的比较恶心，要注意调用这个函数必须循环调用，直到返回CMNERR_NO_DATA为止。
	 * or will ocuurr error......!!!!!!!!!!!!
	 * */
	int32_t CRmpSock::get_recved_data(nm_mem::mem_ptr_t &pMem)
	{
		if ((NULL != m_pMem) && (m_pMem->get_len() > 0))
		{
			pMem = m_pMem;
			m_pMem = NULL;
			return CMNERR_SUC;
		}
		///
		if (m_ui32ValidPkgBegin <= m_ui32ValidPkgEnd)
		{
			pMem = m_vecUnorderedPkgs[m_ui32ValidPkgBegin % m_vecUnorderedPkgs.capacity()];
			m_vecUnorderedPkgs[m_ui32ValidPkgBegin % m_vecUnorderedPkgs.capacity()] = NULL;
			m_ui32ValidPkgBegin++;
			return CMNERR_SUC;
		}
		else
		{
			m_ui32ValidPkgBegin = m_ui32ValidPkgEnd = 0;
		}

		return CMNERR_NO_DATA;
	}

	///
	int32_t CRmpSock::handle_odata()
	{
		SRmpOdata *pOdata = (SRmpOdata*) (m_pMem->get_data());
		if (pOdata->ui32SeqNo == (1 + m_ui32LatestRecvedValidSeqNo))
		{
			///
			m_pMem->dec_head_data(sizeof(SRmpOdata));
			m_ui32LatestRecvedValidSeqNo++;
			///activate the wrong ordered data
			//|----------------|latestrecvvaliddata-----|lostdatabegin-----|lostdataend
			if ((m_ui32LatestRecvedValidSeqNo + 1) == m_ui32UnvalidPkgBegin)
			{
				m_ui32ValidPkgBegin = m_ui32ValidPkgEnd = m_ui32UnvalidPkgBegin;

				///find the valid pkg end
				while (m_ui32ValidPkgEnd <= m_ui32UnvalidPkgEnd)
				{
					if (NULL == m_vecUnorderedPkgs[(m_ui32ValidPkgEnd + 1) % m_vecUnorderedPkgs.capacity()])
					{
						break;
					}
					m_ui32ValidPkgEnd++;
				}

				if (m_ui32ValidPkgEnd < m_ui32UnvalidPkgEnd)
				{
					m_ui32UnvalidPkgBegin = m_ui32ValidPkgEnd + 1;
					while (m_ui32UnvalidPkgBegin <= m_ui32UnvalidPkgEnd)
					{
						if (NULL != m_vecUnorderedPkgs[m_ui32UnvalidPkgBegin % m_vecUnorderedPkgs.capacity()])
						{
							break;
						}
						m_ui32UnvalidPkgBegin++;
					}
				}
				else
				{
					m_ui32UnvalidPkgBegin = m_ui32UnvalidPkgEnd = 0;
				}

				m_ui32LatestRecvedValidSeqNo = m_ui32ValidPkgEnd;
			}
		}
		else if (pOdata->ui32SeqNo <= m_ui32LatestRecvedValidSeqNo)
		{
			//repeated pkg,discard it
			m_pMem->reset();
		}
		else //ui32SeqNo > (m_ui32LatestRecvedSeqNo+1)
		{
			///lost some pkg or wrong ordered.
			///modify begin .end?
			bool bFlag = true;
			if (pOdata->ui32SeqNo < m_ui32UnvalidPkgBegin)
			{
				m_ui32UnvalidPkgBegin = pOdata->ui32SeqNo;
			}
			else if (pOdata->ui32SeqNo > m_ui32UnvalidPkgEnd)
			{
				if ((pOdata->ui32SeqNo - m_ui32LatestRecvedValidSeqNo) > m_vecUnorderedPkgs.capacity())
				{
					///beyond limitation.
					///discard it
					bFlag = false;
					m_pMem->reset();
				}
				else
				{
					m_ui32UnvalidPkgEnd = pOdata->ui32SeqNo;
				}
			}
			///
			if (bFlag)
			{
				CMN_ASSERT(NULL == m_vecUnorderedPkgs[pOdata->ui32SeqNo % m_vecUnorderedPkgs.capacity()]);
				m_vecUnorderedPkgs[pOdata->ui32SeqNo % m_vecUnorderedPkgs.capacity()] = m_pMem;
				m_pMem = NULL;
			}
		}

		return CMNERR_SUC;
	}

	/**
	 * handle heart beat from sender.
	 * */
	int32_t CRmpSock::handle_hb()
	{
		int32_t i32Ret = CMNERR_SUC;
		SRmpHb *pHb = (SRmpHb*) (m_pMem->get_data());
		if (pHb->ui32LatestSeqNo > m_ui32LatestRecvedValidSeqNo)
		{
			SRmpNak nak;
			nak.hdr.ui24Len = sizeof(SRmpNak);
			nak.hdr.ui8Opcode = EP_NAK;
			nak.ui32Begin = m_ui32LatestRecvedValidSeqNo + 1;
			nak.ui32End = pHb->ui32LatestSeqNo;
			nak.ui64Id = m_epid.ui64Id;
			i32Ret = udp_send((cmn_byte_t*)&nak, sizeof(nak), (const struct sockaddr*) (&m_addrSender));
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
	int32_t CRmpSock::handle_nak()
	{

	}

	/**
	 *
	 * */
	int32_t CRmpSock::handle_ack()
	{

	}

	/**
	 *
	 * */
	int32_t CRmpSock::handle_sendep_data()
	{
		///
		SRmpHdr *pHdr = (SRmpHdr*) (m_pMem->get_data());
		IF_TRUE_THEN_RETURN_CODE((pHdr->ui8Opcode >= EP_ALL), CMNERR_COMMON_ERR);
		m_pMem->dec_head_data(sizeof(SRmpHdr));
		return (this->*(pkg_handlers[pHdr->ui8Opcode].fun))();
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
			if (((m_ui32SendingSeqNo + 1) == m_ui32ValidSendingDataTail))
			{
				i32Ret = CMNERR_SUC;
				break;
			}
			///
			CMN_ASSERT(m_ui32SendingSeqNo < m_ui32ValidSendingDataTail);
			///
			i32Ret = udp_send(m_vecSendWin[m_ui32SendingSeqNo + 1].m_pMem, (const struct sockaddr*) (&m_addrMulticast));
			if ((CMNERR_IO_ERR == i32Ret) || (CMNERR_SEND_PENDING == i32Ret))
			{
				break;
			}
			m_ui32SendingSeqNo++;
		}

		return i32Ret;
	}
}
