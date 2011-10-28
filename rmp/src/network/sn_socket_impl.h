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
		int32_t open(sock_handle_t sockhandle = INVALID_SOCKET);
		int32_t close();
		int32_t bind(CIpv4Addr &bindaddr);
		int32_t bind(const cmn_string_t &strBindIP, u_int16_t ui16BindPort);
		int32_t listen(int32_t i32Backlog);
		tcp_sock_ptr_t accept();
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
}

#endif /* SOCKET_H_ */
