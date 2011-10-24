/*
 * socket.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef __SN_SOCKET_IMPL_H__
#define __SN_SOCKET_IMPL_H__

#include <deque>

#include <memory/mem.h>
#include <utils/smart_lock.h>

#if (__USING_LOCK_FREE_CONTAINER__)
#include <intrusive/optimistic_queue.h>
#endif

#include "../common/sn_common.h"
#include "sn_socket.h"

namespace nm_network
{

	/**
	 * tcp socket
	 *
	 * */
	class CTcpSock;
	typedef nm_utils::CSmartPtr<nm_network::CTcpSock> tcp_sock_ptr_t;
	class CTcpSock: public nm_framework::ISocket
	{
		struct SSendInfo
		{
			SSendInfo(nm_mem::mem_ptr_t &ptr) :
				m_pData(ptr), /*m_uiLen(ptr->get_len()),*/
				m_uiOffset(ptr->get_offset())
			{
			}
			SSendInfo(const SSendInfo &other) :
				m_pData(other.m_pData) /*, m_uiLen(other.m_uiLen)*/, m_uiOffset(other.m_uiOffset)
			{
			}

			u_int32_t get_offset()
			{
				return m_uiOffset;
			}

			void set_offset(u_int32_t uiOffset)
			{
				m_uiOffset = uiOffset;
			}

			nm_mem::mem_ptr_t m_pData;
			u_int32_t m_uiOffset;
		};

	public:
		CTcpSock();
		virtual ~CTcpSock();

	public:
		virtual int32_t open(sock_handle_t sockhandle = INVALID_SOCKET);
		virtual int32_t close();
		virtual int32_t bind(CIpv4Addr &bindaddr);
		virtual int32_t bind(const cmn_string_t &strBindIP, u_int16_t ui16BindPort);
		virtual int32_t listen(int32_t i32Backlog);
		virtual tcp_sock_ptr_t accept();
		virtual int32_t connect(const CIpv4Addr &remoteAddr);
		virtual int32_t connect(const cmn_string_t &strAcceptorIp, u_int64_t ui16AcceptorPort);
		virtual sock_handle_t get_handle();
		virtual bool is_opened();
		virtual int32_t set_nonblock(bool bFlag);
		virtual CIpv4Addr& get_peer_addr();
		virtual CIpv4Addr& get_local_addr();

		//virtual int32_t sendex(nm_mem::mem_ptr_t&);
		virtual int32_t send(nm_mem::mem_ptr_t&);
		virtual int32_t send(cmn_pvoid_t pV, u_int32_t ui32Len);

		virtual int32_t handle_can_recv(u_int32_t uiMemSize);
		virtual int32_t recv(cmn_pvoid_t pV, u_int32_t ui32Size);
		int32_t handle_can_send();
		nm_mem::mem_ptr_t& get_recv_data()
		{
			return m_pRecvData;
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
		nm_mem::mem_ptr_t m_pRecvData;
	};

///**
// * udp socket
// * */
//class CUdpSock: public nm_network::ISocket
//{
//public:
//	CUdpSock();
//	virtual ~CUdpSock();
//};
//
//
///**
// * rmp(reliable multicast protocal) socket
// * */
//class CRmpSock: public nm_network::ISocket
//{
//public:
//	CRmpSock();
//	virtual ~CRmpSock();
//};

}

#endif /* SOCKET_H_ */
