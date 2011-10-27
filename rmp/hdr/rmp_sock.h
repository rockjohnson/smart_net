/*
 * rmp_sock.h
 *
 *  Created on: Oct 27, 2011
 *      Author: rock
 */

#ifndef RMP_SOCK_H_
#define RMP_SOCK_H_

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <vector>
#include <common/common.h>
#include <memory/mem.h>

namespace nm_network
{

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

#if 1
	/**
	 *
	 * */
	class CRmpSock
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
		int32_t join_multicast_group(const cmn_string_t &strMulticastIp);
		sock_handle_t get_handle();
		bool is_opened();
		int32_t set_nonblock(bool bFlag);

		int32_t send(nm_mem::mem_ptr_t&);
		int32_t send(cmn_pvoid_t pV, u_int32_t ui32Len);

		int32_t handle_can_recv(u_int32_t);

		int32_t get_local_bind_addr(struct sockaddr_in &addr);
		int32_t handle_sendep_data();
		int32_t handle_recvep_data();
		int32_t handle_can_send();

		int32_t handle_odata();
		int32_t handle_hb();
		nm_mem::mem_ptr_t& get_recved_data();

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
				u_int16_t ui16BindPort;
			};
			u_int64_t ui64Id;
		} m_epid;
	};
#endif
}

#endif /* RMP_SOCK_H_ */
