/*
 * socket.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef __SN_SOCKET_IMPL_H__
#define __SN_SOCKET_IMPL_H__

#include <deque>
#include <vector>
#include <common/common.h>
#if (__USING_GOOGLE_MAP__)
#include <google/dense_hash_map>
#else
#include <map>
#endif

#include <memory/mem.h>
#include <utils/smart_lock.h>

#if (__USING_LOCK_FREE_CONTAINER__)
#include <intrusive/optimistic_queue.h>
#endif

//#include "../common/sn_common.h"
#include "sn_socket.h"

#define __MAX_NO_ANCK_TIME_US__ (5000000)

namespace nm_network
{
	using nm_mem::mem_ptr_t;
	/**
	 * rup socket
	 *
	 * */
	class CRupSock;
	typedef nm_utils::CSmartPtr<nm_network::CRupSock> rup_sock_ptr_t;
	class CRupSock: public ISocket
	{
//		struct SSendInfo
//		{
//			SSendInfo(nm_mem::mem_ptr_t &ptr) :
//				m_pData(ptr), /*m_uiLen(ptr->get_len()),*/
//				m_uiOffset(ptr->get_offset())
//			{
//			}
//			SSendInfo(const SSendInfo &other) :
//				m_pData(other.m_pData) /*, m_uiLen(other.m_uiLen)*/, m_uiOffset(other.m_uiOffset)
//			{
//			}
//
//			u_int32_t get_offset()
//			{
//				return m_uiOffset;
//			}
//
//			void set_offset(u_int32_t uiOffset)
//			{
//				m_uiOffset = uiOffset;
//			}
//
//			nm_mem::mem_ptr_t m_pData;
//			u_int32_t m_uiOffset;
//		};

	public:
		CRupSock();
		virtual ~CRupSock();

	public:
		int32_t open(sock_handle_t sockhandle = INVALID_SOCKET);
		int32_t close();
		int32_t bind(CIpv4Addr &bindaddr);
		int32_t bind(const cmn_string_t &strBindIP, u_int16_t ui16BindPort);
		int32_t listen(int32_t i32Backlog);
		rup_sock_ptr_t accept();
		int32_t connect(const CIpv4Addr &remoteAddr);
		int32_t connect(const cmn_string_t &strAcceptorIp, u_int64_t ui16AcceptorPort);
		sock_handle_t get_handle();
		bool is_opened();
		int32_t set_nonblock(bool bFlag);
		CIpv4Addr& get_peer_addr();
		CIpv4Addr& get_local_addr();

		//virtual int32_t sendex(nm_mem::mem_ptr_t&);
		int32_t send(nm_mem::mem_ptr_t&);
		int32_t send(cmn_pvoid_t pV, u_int32_t ui32Len);

		int32_t handle_can_recv(u_int32_t uiMemSize);
		int32_t recv(cmn_pvoid_t pV, u_int32_t ui32Size);
		int32_t handle_can_send();
		nm_mem::mem_ptr_t& get_recv_data()
		{
			return m_pRecvedData;
		}

	private:
		CIpv4Addr m_localaddr;
		CIpv4Addr m_peeraddr;
		sock_handle_t m_hSock;
		nm_utils::CMutexLock m_lkSending;
		nm_utils::CSpinLock m_lkSendQ;
		typedef std::deque<nm_mem::mem_ptr_t> mem_queue_t;
		mem_queue_t m_qSendCache;
		mem_queue_t m_qSending;
		nm_mem::mem_ptr_t m_pRecvedData;
	};

	/**
	 *
	 * */
#define __MAX_IO_VEC_CNT__ (10)
#define __UNORDERED_PKGS_CNT__ (100000)
#define __SEND_WIN_SIZE__ (1000000)

	typedef sockaddr_in sn_sock_addr_t;
	enum
	{
		RMP_RECV_SOCK = 0, RMP_SEND_SOCK
	};
	class CRmpSock: public ISocket
	{
		typedef std::vector<nm_mem::mem_ptr_t> mem_vec_t;
	public:
		CRmpSock(int32_t i32Type);
		~CRmpSock();

	public:
		int32_t open(sock_handle_t hSock);
		int32_t open(const cmn_string_t &);
		int32_t close();
		int32_t bind(const cmn_string_t &strBindIP, u_int16_t ui16BindPort);
		int32_t join_multicast_group();
		int32_t leave_multicast_group();
		sock_handle_t get_handle();
		bool is_opened();
		int32_t set_nonblock(bool bFlag);

		int32_t send(nm_mem::mem_ptr_t&);
		int32_t send(cmn_pvoid_t pV, u_int32_t ui32Len);

		int32_t handle_can_recv(u_int32_t ui32MemSz);

		int32_t get_local_bind_addr(sn_sock_addr_t&);
		int32_t sendep_handle_data(sn_sock_addr_t&);
		int32_t recvep_handle_data(sn_sock_addr_t&);
		int32_t handle_can_send();
		///
		int32_t recvep_handle_odata(sn_sock_addr_t&);
		int32_t recvep_handle_hb(sn_sock_addr_t&);
		int32_t sendep_handle_nak(sn_sock_addr_t&);
		int32_t sendep_handle_ack(sn_sock_addr_t&);
		int32_t get_next_recved_data(nm_mem::mem_ptr_t &pMem);
		void set_ack();

	private:
		int32_t udp_send(nm_mem::mem_ptr_t &pMem, const struct sockaddr* pDestAddr);
		int32_t udp_send(cmn_byte_t *pBytes, u_int32_t ui32Bytes, const struct sockaddr* pDestAddr);
		int32_t udp_recv(mem_ptr_t &pMem, struct sockaddr_in &sSrcAddr);

	private:
		cmn_string_t m_strBindIp;
		u_int16_t m_ui16BindPort;
		//cmn_string_t m_strMulticastIp;
		sock_handle_t m_hSock;
		int32_t m_i32Type;

		union
		{
			struct
			{
				u_int32_t ui32BindIp;
				u_int16_t ui16BindPort;
			};
			u_int64_t ui64Id;
		} m_epid;

		/*recv endpoint*/
		/*-----------------------------------------------------------------*/
		mem_ptr_t m_pMem; ///which store the recved data.
		u_int8_t m_ui8SenderId; ///should be set by app level.
		volatile u_int64_t m_ui64ValidPkgBegin; ///
		volatile u_int64_t m_ui64ValidPkgEnd;
		sn_sock_addr_t m_addrSender;

		u_int64_t m_ui64LatestRecvedValidSeqNo;
		u_int64_t m_ui64SendAckCnt; ///should be set by app level...
		volatile u_int64_t m_ui64AppConfirmAck; ///经过应用层确认的ack。
		volatile u_int64_t m_ui64AppConfirmAckTmp; ///上次发送ack时的记录数。
		u_int64_t m_ui64UnvalidPkgBegin; ///the first data in the unvalid data vec.
		u_int64_t m_ui64UnvalidPkgEnd; ///the last data in the unvalid data vec.
		/*-----------------------------------------------------------------*/

		/*send endpoint*/
		/*-----------------------------------------------------------------*/
		struct SPkgInfo
		{
			nm_mem::mem_ptr_t m_pMem;
			int32_t i32Acks;
		};
		std::vector<mem_ptr_t> m_vecSendWin;
		nm_utils::CSpinLock m_lkSenderWin;
		volatile u_int64_t m_ui64ValidSendingDataHead; ///最旧的没有接受到足够ack的包序列号
		volatile u_int64_t m_ui64ValidSendingDataTail; ///最近一次成功放入发送窗口的包的下一个序号
		volatile u_int64_t m_ui64SendingSeqNo; ///记录目前已经组播发送出去的包序列号
		volatile u_int64_t m_ui64PkgSeqNoGenerator; ///发送包的序列号生成记录器
		struct sockaddr_in m_addrMulticast;
		u_int64_t m_ui64MaxKeepAliveTimeUs;
		u_int32_t m_ui32Naks;
		u_int32_t m_ui32SendSpeed; ///should set by app level first...
		std::vector<nm_mem::mem_ptr_t> m_vecUnvalidPkgs;
		struct SRecverInfo
		{
			SRecverInfo()
			:ui64LastRecvedSeqNo(0), ui32Naks(0), ui64LastUpdateTimeUs(0){}

			u_int64_t ui64LastRecvedSeqNo;
			u_int32_t ui32Naks;
			u_int64_t ui64LastUpdateTimeUs;
		};
#if (__USING_GOOGLE_MAP__)
		typedef google::dense_hash_map<u_int64_t, SRecverInfo> recver_map_t;
#else
		typedef std::map<u_int64_t, SRecverInfo> recver_map_t;
#endif
		recver_map_t m_mapRecvers;
		/*-----------------------------------------------------------------*/
	};
	typedef nm_utils::CSmartPtr<nm_network::CRmpSock> rmp_sock_ptr_t;
}

#endif /* SOCKET_H_ */
