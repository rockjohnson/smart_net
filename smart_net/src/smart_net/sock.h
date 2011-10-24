/*
* sock.h
*
*  Created on: Apr 5, 2009
*      Author: rock
*/

#ifndef __SOCK_H__
#define __SOCK_H__

#if defined(__PLATEFORM_LINUX__)
#include <sys/socket.h>
//#include <netinet/in.h>
#include <arpa/inet.h>
#elif defined(__PLATEFORM_WINDOWS__)
typedef int socklen_t;
#define EWOULDBLOCK WSAEWOULDBLOCK
#define EINPROGRESS WSAEWOULDBLOCK
#define EMSGSIZE WSAEMSGSIZE
#endif


#include <string>
#include <deque>
#include "type.h"
#include "mem.h"
#include "auto_lock.h"
#include "defines.h"

namespace nm_network
{
	using namespace nm_utils;

	class SOFT_NET_API CSocket
	{
	public:
		enum
		{
			HAVE_ERR = -1,
			OPER_IS_DONE = 0,
			SEND_IS_PENDING = OPER_IS_DONE + 1,
			RECV_IS_PENDING = SEND_IS_PENDING + 1,
			MSG_SIZE_ERR = RECV_IS_PENDING + 1
		};

		class SOFT_NET_API CAddr
		{
		public:
			CAddr():m_uiIp(0), m_usPort(0){}

		public:
			void set_ne_ip(ui32_t uiIp/*network byte order*/)
			{
				m_uiIp = uiIp;
			}
			ui32_t get_ne_ip()
			{
				return m_uiIp;
			}
			void set_ne_port(us16_t usPort/*network endian*/)
			{
				m_usPort = usPort;
			}
			void set_he_port(us16_t usPort/*host endian*/)
			{
				m_usPort = htons(usPort);
			}
			us16_t get_ne_port()
			{
				return m_usPort;
			}

			const char* get_str_ip()
			{
				struct in_addr addr = {0};
				addr.s_addr = m_uiIp;
				return ::inet_ntoa(addr);
			}

			void set_str_ip(const char *pcszIp)
			{
#if defined(__PLATEFORM_LINUX__)
				struct in_addr addr = { 0 };
				ASSERT(0 != ::inet_aton(pcszIp, &addr));
				set_ne_ip(addr.s_addr);
#elif defined(__PLATEFORM_WINDOWS__)
				ASSERT(INADDR_NONE != ::inet_addr(pcszIp));
				set_ne_ip(::inet_addr(pcszIp));
#endif
			}

			us16_t get_he_port()
			{
				return ntohs(m_usPort);
			}

		private:
			ui32_t m_uiIp; //network byte order
			us16_t m_usPort; //network byte order
		};

	protected:
		CSocket(int iType);
		virtual ~CSocket();

	public:
		virtual int asyn_send(mem_ptr_t &ptr) = 0;
		virtual int asyn_send(mem_queue_t &mq) = 0;
		virtual int handle_can_send() = 0;
		virtual int handle_can_recv(ui32_t uiMemSize) = 0;

		int close();
		bool is_opened();

		//bool get_recv_data(mem_ptr_t &ptr);//not erase it
		//virtual mem_queue_t& get_recv_data() = 0;
		//void pop_recv_data(); //pop current data;
		//ui32_t get_total_data_len();

		int get_handle() const;
		void set_handle(int iSock);
		int bind_he(cstr_t, us16_t usPort/*host endian*/);
		int	bind_ne(ui32_t uiIp/*network byte order*/, us16_t usPort/*network byte order*/);
		int listen(int backlog = 20);
		int get_type()
		{
			return m_iType;
		}
		int create();
		int set_nonblock_flag(int value);
		//bool is_valid();
		virtual void clear_buf() = 0;//not thread safe, please invoke this function in the reclaim function or some thread safe time.
		void reset();

		//
		int get_linger(us16_t &usSwitch, us16_t &usSecs);
		int set_linger(us16_t usSwitch, us16_t usSecs);
		//
		int get_send_buf_size();
		int set_send_buf_size(int iSize);
		int get_recv_buf_size();
		int set_recv_buf_size(int iSize);

		virtual bool if_exist_pending_send_data() = 0;

		//
		//		void set_peer_ip(ui32_t uiIp/*network byte order*/)
		//		{
		//			m_uiPeerIp = uiIp;
		//		}
		//		//void set_peer_ip(const char *pcszIp);
		//		ui32_t get_peer_ip()
		//		{
		//			return m_uiPeerIp;
		//		}
		//		//const char* get_str_ip();
		//		void set_peer_port(us16_t usPort/*network byte order*/)
		//		{
		//			m_usPeerPort = usPort;
		//		}
		//		us16_t get_peer_port()
		//		{
		//			return m_usPeerPort;
		//		}
		//		us16_t get_host_endian_port()
		//		{
		//			return ::ntohs(m_usPeerPort);
		//		}

		//
		//		void set_local_ip(ui32_t uiIp/*network byte order*/)
		//		{
		//			m_uiLocalIp = uiIp;
		//		}
		//		ui32_t get_local_ip()
		//		{
		//			return m_uiLocalIp;
		//		}
		//		void set_local_port(us16_t usPort/*network byte order*/)
		//		{
		//			m_usLocalPort = usPort;
		//		}
		//		us16_t get_local_port()
		//		{
		//			return m_usLocalPort;
		//		}

		CAddr& get_local_addr()
		{
			return m_local_addr;
		}
		CAddr& get_peer_addr()
		{
			return m_peer_addr;
		}

	protected:
		//
		CAutoLock m_lock_close;
#if defined(__PLATEFORM_LINUX__)
		int m_iSock;
#elif defined(__PLATEFORM_WINDOWS__)
		SOCKET m_iSock;
#endif
		//
		int m_iType;
		//
		CAddr m_peer_addr;
		CAddr m_local_addr;
	};

	class SOFT_NET_API CTcpSock: public CSocket
	{
		//private:
		struct SSendInfo
		{
			SSendInfo(mem_ptr_t &ptr) :
		m_pData(ptr), /*m_uiLen(ptr->get_len()),*/
			m_uiOffset(ptr->get_offset())
		{
		}
		SSendInfo(const SSendInfo &other) :
		m_pData(other.m_pData) /*, m_uiLen(other.m_uiLen)*/, m_uiOffset(
			other.m_uiOffset)
		{
		}

		ui32_t get_offset()
		{
			return m_uiOffset;
		}

		void set_offset(ui32_t uiOffset)
		{
			m_uiOffset = uiOffset;
		}

		mem_ptr_t m_pData;
		ui32_t m_uiOffset;
		};
	public:
		CTcpSock();
		~CTcpSock();

	public:
		int asyn_send(mem_ptr_t &ptr);
		int asyn_send(mem_queue_t &mq);
		int handle_can_send();
		int handle_can_recv(ui32_t uiMemSize);
		int accept_new_sock(ui32_t &uiIp, us16_t &usPort);
		int asyn_connect(std::string &strIp, us16_t usPort);
		int asyn_connect(ui32_t uiIp, us16_t usPort);
		void clear_buf();
		mem_queue_t& get_recv_data();
		ui32_t get_recv_data_len();

		bool if_exist_pending_send_data(){return !m_qSendCache.empty();}

	private:
		int __asyn_send(mem_ptr_t &ptr);

	private:
		CAutoLock m_lock_send;
		std::deque<SSendInfo> m_qSendCache;
		//
		std::deque<mem_ptr_t> m_qRecving;
	};

	///
	class CUdpSock: public CSocket
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
		//ui32_t get_recv_data_len();

		bool if_exist_pending_send_data() {return !m_queue_send.empty();}

	private:
		int __asyn_send(mem_ptr_t &ptr, ui32_t uiIp, us16_t usPort);
		void clear_buf();

	private:
		CAutoLock m_lock_send;
		std::deque<SInfo> m_queue_send;
		//
		std::deque<SInfo> m_queue_recv;
	};
}

#endif /* __SOCK_H__ */
