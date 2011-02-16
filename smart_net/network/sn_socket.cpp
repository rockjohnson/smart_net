/*
 * socket.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#include "sn_socket.h"
#if __PLATFORM__ == __PLATFORM_LINUX__
#include <sys/types.h>
#include <sys/socket.h>
#endif
#include <utils/utils.h>

namespace nm_network
{

using namespace nm_utils;

ISocket::ISocket()
{
	// TODO Auto-generated constructor stub

}

ISocket::~ISocket()
{
	// TODO Auto-generated destructor stub
}

/**
 * tcp socket.
 * */

CTcpSock::CTcpSock()
{
	// TODO Auto-generated constructor stub

}

CTcpSock::~CTcpSock()
{
	// TODO Auto-generated destructor stub
}

int32_t CTcpSock::create(int32_t i32fd)
{
	SYS_ASSERT(!is_valid());
	if (is_valid())
	{
		destroy();
	}

	if (0 <= i32fd)
	{
		m_i32fd = i32fd;
	}
	else
	{
		m_i32fd = socket(AF_INET, SOCK_STREAM, 0);
	}

	return 0 > m_i32fd ? SNERR_CREAT_SOCK_FAILDED : CMNERR_SUC;
}

int32_t CTcpSock::close()
{
	if (INVALID_SOCKET < m_i32fd)
	{
		close(m_i32fd);
		m_i32fd = INVALID_SOCKET;
	}

	return CMNERR_SUC;
}

int32_t CTcpSock::set_nonblock(bool bFlag)
{
	return set_block_flag(m_i32fd, !bFlag);
}

int32_t CTcpSock::bind(INetAddr &localAddr)
{
	SYS_ASSERT(INVALID_SOCKET < m_i32fd);

	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = HTONS(localAddr.get_port());
	bindAddr.sin_addr.s_addr = HTONL(localAddr.get_ip());

	return bind(m_i32fd, (struct sockaddr*)&bindAddr, sizeof(bindAddr));
}

int32_t CTcpSock::listen(int32_t i32Backlog)
{
	return listen(m_i32fd, i32Backlog);
}

CTcpSock::tcp_sock_ptr_t CTcpSock::accept()
{
	tcp_sock_ptr_t pTcpSock;
	struct sockaddr_in remoteAddr;
	ZERO_MEM(&remoteAddr);
	socklen_t slSize = sizeof(remoteAddr);
	int32_t i32NewSockFd = accept(m_i32fd, static_cast<struct sockaddr*>(&remoteAddr), &slSize);
	if (-1 == i32NewSockFd)
	{
		return pTcpSock;
	}
	pTcpSock = SYS_NOTRW_NEW(CTcpSock);
	SYS_ASSERT(NULL != pTcpSock);
	pTcpSock->create(i32NewSockFd);

	return pTcpSock;
}

int32_t CTcpSock::connect(INetAddr &remoteAddr)
{
	struct sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = HTONS(remoteAddr.get_port());
	destAddr.sin_addr.s_addr = HTONL(remoteAddr.get_ip());

	int32_t i32Ret = connect(m_i32fd, static_cast<struct sockaddr*>(&destAddr), sizeof(destAddr));

	return CMNERR_SUC == i32Ret ? CMNERR_SUC : (EINPROGRESS == errno ? SNERR_IN_PROGRESS : CMNERR_COMMON_ERR);
}


/**
 * udp ..
 * */
CUdpSock::CUdpSock()
{
	// TODO Auto-generated constructor stub

}

CUdpSock::~CUdpSock()
{
	// TODO Auto-generated destructor stub
}

CRmpSock::CRmpSock()
{
	// TODO Auto-generated constructor stub

}

CRmpSock::~CRmpSock()
{
	// TODO Auto-generated destructor stub
}

}
