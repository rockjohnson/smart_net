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

#if 0
	/**
	 *
	 * */
	class CUdpSock: public nm_framework::ISocket
	{
	public:
		struct SInfo
		{
		public:
			SInfo(mem_ptr_t &ptr, ui32_t uiIp, us16_t usPort) :
			m_uiIp(uiIp), m_usPort(usPort), m_ptr_mem(ptr)
			{
			}

			explicit SInfo(const SInfo &other) :
			m_uiIp(other.m_uiIp), m_usPort(other.m_usPort), m_ptr_mem(other.m_ptr_mem)
			{
			}

			ui32_t get_ip()
			{
				return m_uiIp;
			}

			us16_t get_port()
			{
				return m_usPort;
			}

			mem_ptr_t& get_mem()
			{
				return m_ptr_mem;
			}

		private:
			ui32_t m_uiIp;//network byte order
			us16_t m_usPort;//network byte order
			mem_ptr_t m_ptr_mem;
		};
		typedef std::deque<SInfo> info_queue_t;

	public:
		CUdpSock();
		~CUdpSock();

	public:
		int asyn_send(mem_ptr_t &ptr);
		int asyn_send(mem_queue_t &mq);
		int asyn_send(mem_ptr_t &ptr, ui32_t uiIp, us16_t usPort);
		int asyn_send(mem_queue_t &mq, ui32_t uiIp, us16_t usPort);
		int handle_can_send();
		int handle_can_recv(ui32_t uiMemSize);

		info_queue_t& get_recv_data();

		bool if_exist_pending_send_data()
		{
			return !m_queue_send.empty();
		}

	private:
		int __asyn_send(mem_ptr_t &ptr, ui32_t uiIp, us16_t usPort);
		void clear_buf();

	private:
		CAutoLock m_lock_send;
		std::deque<SInfo> m_queue_send;
		//
		std::deque<SInfo> m_queue_recv;
	};
#endif

#if 0
	/**
	 *
	 * */
	class CRmpSock: public nm_framework::ISocket
	{
		enum
		{
			E_RECV_ENDPOINT = 0, E_SEND_ENDPOINT
		};

	public:
		CRmpSock();
		~CRmpSock();

	public:
		int32_t open(sock_handle_t hSock);
		int32_t close();
		int32_t bind(const cmn_string_t &strBindIP, u_int16_t ui16BindPort);
		int32_t join_multicast_group(const cmn_string_t strMulticastIp);
		sock_handle_t get_handle();
		bool is_opened();
		int32_t set_nonblock(bool bFlag);

		int32_t send(nm_mem::mem_ptr_t&) = 0;
		int32_t send(cmn_pvoid_t pV, u_int32_t ui32Len) = 0;

		int32_t handle_can_recv(u_int32_t);

	private:
		int32_t handle_odata();
		int32_t handle_hb();

	private:
		cmn_string_t m_strBindIp;
		u_int16_t m_ui16BindPort;
		cmn_string_t m_strMulticastIp;
		sock_handle_t m_hSock;
		int32_t m_i32Type;
		nm_mem::mem_ptr_t m_pMem;
		u_int32_t m_ui32LatestRecvedValidSeqNo;
		u_int32_t m_ui32UnvalidPkgBegin;
		u_int32_t m_ui32UnvalidPkgEnd;
		u_int32_t m_ui32ValidPkgBegin;
		u_int32_t m_ui32ValidPkgEnd;
		std::vector<nm_mem::mem_ptr_t> m_vecUnorderedPkgs;
		struct sockaddr_in m_addrSender;
		union
		{
			struct
			{
				u_int32_t ui32BindIp;
				u_int16_t ui32BindPort;
			};
			u_int64_t ui64Id;
		} m_epid;
	};
#endif

}

#endif /* SOCKET_H_ */
