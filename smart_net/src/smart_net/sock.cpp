/*
 * sock.cpp
 *
 *  Created on: Apr 5, 2009
 *      Author: rock
 */
#include "plateform.h"

#if defined(__PLATEFORM_LINUX__)
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#elif defined(__PLATEFORM_WINDOWS__)
#include <WinSock2.h>
#endif
#include <sys/types.h>
#include <fcntl.h>

#include "../hdr/defines.h"
#include "../hdr/sock.h"

#include "singleton.h"
#include "log.h"

namespace nm_network
{
	using namespace nm_utils;

	CSocket::CSocket(int iType)
	:m_iSock(INVALID_SOCKET), m_iType(iType)
	{
		// TODO Auto-generated constructor stub
	}

	int CSocket::create()
	{
		ASSERT(m_iSock == INVALID_SOCKET);
		m_iSock = socket(AF_INET, m_iType, 0);
		if (m_iSock < 0)
		{
			TRACE_LAST_ERR(socket);
			ASSERT(m_iSock > -1);
		}

		TRACE_LOG(LOG, ELL_DEBUG, L"create socket %d\n", m_iSock);

#ifdef __FOR_DEBUG__
		if (SOCK_STREAM == m_iType) //on windows, the udp doesn't support this function.
		{
			//set_linger(1, 10);
			us16_t usSwitch = 0, usSecs = 0;
			get_linger(usSwitch, usSecs);
			TRACE_LOG(LOG, ELL_DEBUG, L"default socket linger config: %hu, %hu\n", usSwitch, usSecs);
			TRACE_LOG(LOG, ELL_DEBUG, L"default socket send buffer size is %d, recv buffer size is %d.\n",
				get_send_buf_size(), get_recv_buf_size());
		}
#endif

		return 0;
	}

	CSocket::~CSocket()
	{
		// TODO Auto-generated destructor stub
		ASSERT(m_iSock == INVALID_SOCKET);
	}

//	bool CBaseSock::is_valid()
//	{
//		//ScopeLock lock(m_lock_close);
//		return m_iSock == INVALID_SOCKET ? false : true;
//	}

	//not thread safe.
	int CSocket::close()
	{
		if (!m_lock_close.try_lock())
		{
			return 1;
		}
		//m_lock_close.lock();
		if (m_iSock == INVALID_SOCKET)
		{
			m_lock_close.unlock();
			return 2;
		}
		int i = m_iSock;
		m_iSock = INVALID_SOCKET;
		m_lock_close.unlock();

		TRACE_LOG(LOG, ELL_DEBUG, L"close socket %d\n", i);
		//ep.remove_sock(i);
#if defined(__PLATEFORM_LINUX__)
		::close(i);
#elif defined(__PLATEFORM_WINDOWS__)
		::closesocket(i);
#endif
		return 0;
	}

//	void CSocket::clear_buf()
//	{
//		m_queue_recv.clear();
//
//		m_lock_send.lock();
//		m_queue_send.clear();
//		m_lock_send.unlock();
//	}

	void CSocket::reset()
	{
		ASSERT(m_iSock >= 0);
		m_iSock = INVALID_SOCKET;
	}

	bool CSocket::is_opened()
	{
		return m_iSock != INVALID_SOCKET;
	}

	int CSocket::get_linger(us16_t &usSwitch, us16_t &usSecs)
	{
		struct linger lin = {0};
#if defined(__PLATEFORM_WINDOWS__)
		int len = sizeof(lin);
#elif defined(__PLATEFORM_LINUX__)
		socklen_t len = sizeof(lin);
#endif
		if (getsockopt(m_iSock, SOL_SOCKET, SO_LINGER, (char*)&lin, &len) < 0)
		{
			TRACE_LAST_ERR(getsockopt());
			return -1;
		}
		usSwitch = lin.l_onoff;
		usSecs = lin.l_linger;

		return RET_SUC;
	}

	int CSocket::set_linger(us16_t usSwitch, us16_t usSecs)
	{
		struct linger lin = {usSwitch, usSecs};
		if (setsockopt(m_iSock, SOL_SOCKET, SO_LINGER, (const char*)&lin, sizeof(lin)) < 0)
		{
			TRACE_LAST_ERR(setsockopt());
			return -1;
		}

		return RET_SUC;
	}

//	bool CSocket::get_recv_data(mem_ptr_t &ptr)
//	{
//		if (m_queue_recv.empty())
//		{
//			return false;
//		}
//
//		ptr = m_queue_recv.front();
//
//		return true;
//	}

//	mem_queue_t& CSocket::get_recv_data()
//	{
//		return m_queue_recv;
//	}

//	void CSocket::pop_recv_data()
//	{
//		ASSERT(!m_queue_recv.empty());
//		//m_queue_recv.front()->set_offset(0);//the offset for recv is just for recving again, but now we pop it out, so make the offset zero.
//		m_queue_recv.pop_front();
//	}

//	ui32_t CSocket::get_total_data_len()
//	{
//		ui32_t ui = 0;
//		for (std::deque<mem_ptr_t>::iterator iter = m_queue_recv.begin(); iter
//		!= m_queue_recv.end(); ++iter)
//		{
//			ui += (*iter)->get_cur_len();
//		}
//
//		return ui;
//	}

	int CSocket::get_handle() const
	{
		return m_iSock;
	}

	void CSocket::set_handle(int iSock)
	{
		ASSERT(m_iSock == INVALID_SOCKET);
		m_iSock = iSock;
	}

	int CSocket::bind_he(cstr_t strIp, us16_t usPort)
	{
		ASSERT(strIp != NULL);
		ASSERT(is_opened());

		struct sockaddr_in local_addr =
		{ 0 };
		local_addr.sin_family = AF_INET;
		//local_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
#if defined(__PLATEFORM_LINUX__)
		::inet_aton(strIp, &local_addr.sin_addr);
#elif defined(__PLATEFORM_WINDOWS__)
		local_addr.sin_addr.s_addr = inet_addr(strIp);
#endif
		local_addr.sin_port = htons(usPort);

		if (::bind(m_iSock, (const struct sockaddr *) &local_addr,
				sizeof(local_addr)) < 0)
		{
			TRACE_LAST_ERR(bind);
			return HAVE_ERR;
		}

		return OPER_IS_DONE;
	}

	int CSocket::bind_ne(ui32_t uiIp, us16_t usPort)
	{
		ASSERT(is_opened());

		struct sockaddr_in local_addr =
		{ 0 };
		local_addr.sin_family = AF_INET;
		local_addr.sin_addr.s_addr = uiIp;
		local_addr.sin_port = usPort;

		if (::bind(m_iSock, (const struct sockaddr *) &local_addr,
				sizeof(local_addr)) < 0)
		{
			TRACE_LAST_ERR		( bind)
			return HAVE_ERR;
		}

		return OPER_IS_DONE;
	}

	int CSocket::listen(int iBacklog)
	{
		return ::listen(m_iSock, iBacklog);
	}

	/* Set the O_NONBLOCK flag of desc if value is nonzero,
	 or clear the flag if value is 0.
	 Return 0 on success, or -1 on error with errno set. */
	int CSocket::set_nonblock_flag(int value)
	{
#if defined(__PLATEFORM_LINUX__)
		int oldflags = ::fcntl(m_iSock, F_GETFL, 0);
		/* If reading the flags failed, return error indication now. */
		if (oldflags == -1)
			return -1;

		/* Set just the flag we want to set. */
		if (value != 0)
			oldflags |= O_NONBLOCK;
		else
			oldflags &= ~O_NONBLOCK;
		/* Store modified flag word in the descriptor. */
		return ::fcntl(m_iSock, F_SETFL, oldflags);
#elif defined(__PLATEFORM_WINDOWS__)
		if (::ioctlsocket(m_iSock, FIONBIO, (u_long FAR*)&value) == SOCKET_ERROR)
		{
			TRACE_LAST_ERR("setsockopt()");
			return -1;
		}

		return 0;
#endif
	}

	int CSocket::get_send_buf_size()
	{
		int iSize = 0;
#if defined(__PLATEFORM_WINDOWS__)
		int len = sizeof(iSize);
#elif defined(__PLATEFORM_LINUX__)
		socklen_t len = sizeof(iSize);
#endif
		return getsockopt(m_iSock, SOL_SOCKET, SO_SNDBUF, (char*)&iSize, &len) < 0 ? -1 : iSize;
	}

	int CSocket::set_send_buf_size(int iSize)
	{
		return setsockopt(m_iSock, SOL_SOCKET, SO_SNDBUF, (const char*)&iSize, sizeof(iSize));
	}

	int CSocket::get_recv_buf_size()
	{
		int iSize = 0;
		socklen_t len = sizeof(iSize);
		return getsockopt(m_iSock, SOL_SOCKET, SO_RCVBUF, (char*)&iSize, &len) < 0 ? -1 : iSize;
	}

	int CSocket::set_recv_buf_size(int iSize)
	{
		return setsockopt(m_iSock, SOL_SOCKET, SO_RCVBUF, (const char*)&iSize, sizeof(iSize));
	}

//	void CSocket::set_peer_ip(const char *pcszIp)
//	{
//		struct in_addr addr = {0};
//		ASSERT(0 != ::inet_aton(pcszIp, &addr));
//		set_peer_ip(addr.s_addr);
//	}

//	const char* CSocket::get_str_ip()
//	{
//		struct in_addr addr = {m_uiPeerIp};
//		return ::inet_ntoa(addr);
//	}

	///////////////////////////////////////////////////////////////////////////////////////////
	CTcpSock::CTcpSock() :
		CSocket(SOCK_STREAM)
		{
		}

	CTcpSock::~CTcpSock()
	{
	}

	void CTcpSock::clear_buf()
	{
		m_queue_recv.clear();
		//
		m_lock_send.lock();
		m_queue_send.clear();
		m_lock_send.unlock();
	}

	mem_queue_t& CTcpSock::get_recv_data()
	{
		return m_queue_recv;
	}

	ui32_t CTcpSock::get_recv_data_len()
	{
		ui32_t ui = 0;
		for (std::deque<mem_ptr_t>::iterator iter = m_queue_recv.begin();
		iter != m_queue_recv.end(); ++iter)
		{
			ui += (*iter)->get_cur_len();
		}

		return ui;
	}

	int CTcpSock::asyn_send(mem_queue_t &mq)
	{
		if (m_iSock < 0)
		{
			return -10000;
		}

		int iRet = 0;
		CScopeLock<CAutoLock> lock(m_lock_send);

		for (mem_queue_t::iterator iter = mq.begin();
		iter != mq.end(); iter++)
		{
			if ((iRet = __asyn_send(*iter)) < 0)
			{
				return -1;
			}
		}

		return iRet;
	}

	int CTcpSock::asyn_send(mem_ptr_t &ptr)
	{
		if (m_iSock < 0)
		{
			return -10000;
		}

		CScopeLock<CAutoLock> lock(m_lock_send);

		return __asyn_send(ptr);
	}

	int CTcpSock::__asyn_send(mem_ptr_t &ptr)
	{
		ASSERT(ptr->get_cur_len() > 0);

		//if there have been a block of data needed sending, just push me after it.
		if (!m_queue_send.empty())
		{
			//ASSERT(false);
			if (m_queue_send.back().m_ptr->get_ref_cnt() == 1
					&& m_queue_send.back().m_ptr->get_free_size() >= ptr->get_cur_len())
			{
				m_queue_send.back().m_ptr->append(ptr->get_cur_buf(), ptr->get_cur_len());
			}
			else
			{
				m_queue_send.push_back(SSnder(ptr));
			}

			return SEND_IS_PENDING;
		}

		SSnder snder(ptr);
		int iRet = 0;
		for (;;)
		{
			iRet = ::send(m_iSock, (const char*)(ptr->get_buf() + snder.get_offset()),
					ptr->get_len() - snder.get_offset(), 0/*MSG_NOSIGNAL*/);
			if (iRet < 0)
			{
#if defined(__PLATEFORM_LINUX__)
				if (EWOULDBLOCK/*EAGAIN*/==errno)
#elif defined(__PLATEFORM_WINDOWS__)
				if (WSAEWOULDBLOCK/*EAGAIN*/==::GetLastError())
#endif
				{
					//ASSERT(false); //should not reach here
					ASSERT(ptr->get_len() > snder.get_offset());
					m_queue_send.push_back(snder);//cache it.
					iRet = SEND_IS_PENDING;
				}
				else
				{
					//error occurred!
					TRACE_LAST_ERR(send);
					//close_sock();
				}
				break;
			}

#ifdef __FOR_DEBUG__
			if (0 == iRet)
			{
				TRACE_LAST_ERR(send);
				continue;
			}
#endif
			ASSERT(iRet != 0); //what the meaning if zero??

			if (iRet < (ptr->get_len() - snder.get_offset()))
			{
				snder.set_offset(snder.get_offset()+iRet);
				m_queue_send.push_back(snder);//cache it.
				iRet = SEND_IS_PENDING;
				break;
			}

			snder.set_offset(snder.get_offset()+iRet);
			if (snder.get_offset() == ptr->get_len())
			{
				//TRACE_LOG(LOG, ELL_DEBUG, L"%d is sent\n", ptr->get_cur_len());
				//all are sent.
				iRet = RET_SUC;
				break;
			}
		}

		return iRet;
	}

	int CTcpSock::handle_can_send()
	{
		CScopeLock<CAutoLock> lock(m_lock_send);

		int iRet = 0;
		while (!m_queue_send.empty())
		{
			SSnder &snder = m_queue_send.front();
			for(;;)
			{
				iRet = ::send(m_iSock, (const char*)(snder.m_ptr->get_buf()+snder.get_offset()),
						snder.m_ptr->get_len()-snder.get_offset(), 0);
				if (iRet < 0)
				{
#if defined(__PLATEFORM_LINUX__)
					if (EWOULDBLOCK == errno)
#elif defined(__PLATEFORM_WINDOWS__)
					if (WSAEWOULDBLOCK == ::GetLastError())
#endif
					{
						//ASSERT(false);//should not reach here.
						ASSERT(snder.m_ptr->get_len() > snder.get_offset());
						iRet = SEND_IS_PENDING;
					}
					else
					{
						TRACE_LAST_ERR(send);
						//close_sock();
					}
					break;
				}

				ASSERT(iRet != 0);

				if (iRet < (snder.m_ptr->get_len()-snder.get_offset()))
				{
					snder.set_offset(snder.get_offset()+iRet);
					iRet = SEND_IS_PENDING;
					break;
				}

				snder.set_offset(snder.get_offset()+iRet);
				iRet = RET_SUC;
				if (snder.get_offset() == snder.m_ptr->get_len())
				{
					//all are sent.
					m_queue_send.pop_front();
					break;
				}
			}

			if (iRet != RET_SUC)
			{
				break;
			}
		}

		return iRet;
	}

	//no need thread safe, we just invoke this function in single thread.
	int CTcpSock::handle_can_recv(ui32_t uiMemSize)
	{
		int iRet = RET_SUC;
		mem_ptr_t ptr = NULL;

		//if there are some data left.
		if (!m_queue_recv.empty())
		{
			//int i = 0;
			ASSERT((/*i = */m_queue_recv.back()->get_cur_len()) > 0);
			if (m_queue_recv.back()->get_free_size() > (uiMemSize/2))
			{
				ptr = m_queue_recv.back();
				m_queue_recv.pop_back();
			}
		}

		for (;;)
		{
			if (NULL == ptr)
			{
				ptr = NEW_MEM(uiMemSize);
			}

			iRet = ::recv(m_iSock, (char*)ptr->get_free_buf(), ptr->get_free_size(), 0);
			if (iRet == 0)
			{
				//normal shutdown
				TRACE_LOG(LOG, ELL_INFO, L"the peer has performed an orderly shutdown\n");
				iRet = HAVE_ERR;
				break;
			}
			else if (iRet < 0)
			{
#if defined(__PLATEFORM_LINUX__)
				if (EWOULDBLOCK == errno)
#elif defined(__PLATEFORM_WINDOWS__)
				if (WSAEWOULDBLOCK == ::GetLastError())
#endif
				{
					//ASSERT(false); ///should not reach here
					if (ptr->get_cur_len() > 0)
					{
						m_queue_recv.push_back(ptr);
					}
					iRet = RECV_IS_PENDING;
				}
				else
				{
					TRACE_LAST_ERR(recv);
				}
				break;
			}
			else if (iRet < ptr->get_free_size())
			{
				///no more data for receiving
				ptr->inc_len(iRet);
				m_queue_recv.push_back(ptr);
				iRet = RECV_IS_PENDING;
				break;
			}

			ptr->inc_len(iRet);
			if (ptr->get_free_size() == 0)
			{
				m_queue_recv.push_back(ptr);
				ptr = NULL;
			}
		}

		return iRet;
	}

	int CTcpSock::accept_new_sock(ui32_t &uiIp, us16_t &usPort)
	{
		struct sockaddr_in addr =
		{	0};
		int iLen = sizeof(addr);
		int iSock = ::accept(m_iSock, (struct sockaddr*)&addr, (socklen_t*)&iLen);
		TRACE_LOG(LOG, ELL_DEBUG, L"accept new socket: %d\n", iSock);
		if (iSock < 0)
		{
			TRACE_LAST_ERR(accept);
			return -1;
		}

		uiIp = addr.sin_addr.s_addr;
		usPort = addr.sin_port;

		return iSock;
	}

	int CTcpSock::asyn_connect(std::string &strIp, us16_t usPort)
	{
		ASSERT(m_iSock != INVALID_SOCKET);

		set_nonblock_flag(1);

		struct sockaddr_in addr = {0};
		addr.sin_family = AF_INET;
#if defined(__PLATEFORM_LINUX__)
		::inet_aton(strIp.c_str(), &(addr.sin_addr));
#elif defined(__PLATEFORM_WINDOWS__)
		addr.sin_addr.s_addr = ::inet_addr(strIp.c_str());
#endif
		addr.sin_port = htons(usPort);

		if (::connect(m_iSock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		{
#if defined(__PLATEFORM_LINUX__)
			if (EINPROGRESS == errno)
#elif defined(__PLATEFORM_WINDOWS__)
			if (WSAEWOULDBLOCK == ::GetLastError())
#endif
			{
				TRACE_LAST_ERR(connect);
				return 1;
			}
			else
			{
				TRACE_LAST_ERR(connect);
				return -1;
			}
		}
		else
		{
			return 0;//connected.
		}
	}

	int CTcpSock::asyn_connect(ui32_t uiIp, us16_t usPort)
	{
		ASSERT(m_iSock != INVALID_SOCKET);

		set_nonblock_flag(1);

		struct sockaddr_in addr =
		{	0};
		addr.sin_family = AF_INET;
		addr.sin_addr.s_addr = uiIp;
		addr.sin_port = usPort;

		if (::connect(m_iSock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
		{
#if defined(__PLATEFORM_LINUX__)
			if (EINPROGRESS == errno)
#elif defined(__PLATEFORM_WINDOWS__)
			if (WSAEWOULDBLOCK == ::WSAGetLastError())
#endif
			{
				TRACE_LAST_ERR(connect);
				return 1;
			}
			else
			{
				TRACE_LAST_ERR(connect);
				return -1;
			}
		}
		else
		{
			return 0;//connected.
		}
	}

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
		struct sockaddr_in remote_addr = {0};
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
		struct sockaddr_in remote_addr = {0};
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
		struct sockaddr_in remote_addr = {0};
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
					///解决windows平台上两个udp互相发送数据时，其中一个关闭，另一个会获得10054的错误！
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
}
