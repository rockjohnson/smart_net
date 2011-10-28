/*
 * rmp_sock.cpp
 *
 *  Created on: Oct 27, 2011
 *      Author: rock
 */

#include <unistd.h>
#include <utils/utils.h>
#include "../hdr/rmp_sock.h"

namespace nm_network
{

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

#if 1
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
		if (E_RECV_ENDPOINT == i32EpType)
		{
			m_vecUnorderedPkgs.resize(__UNORDERED_PKGS_CNT__);
		}
		else if (E_SEND_ENDPOINT == i32EpType)
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
				if (CMNERR_SUC == m_vecSendWin[m_ui32ValidSendingDataTail - 1].m_pMem->append(pM->get_offset_data(pM->get_init_offset),
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

			if (E_RECV_ENDPOINT == m_i32Type)
			{
				return handle_recvep_data();
			}
			else if (E_SEND_ENDPOINT == m_i32Type)
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
			in32Ret = udp_send(&nak, sizeof(nak), (const struct sockaddr*)(&m_addrSender));
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
			i32Ret = udp_send(m_vecSendWin[m_ui32SendingSeqNo + 1], (const struct sockaddr*)(&m_addrMulticast));
			if ((CMNERR_IO_ERR == i32Ret) || (CMNERR_SEND_PENDING == i32Ret))
			{
				break;
			}
			m_ui32SendingSeqNo++;
		}

		return i32Ret;
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
#endif

}
