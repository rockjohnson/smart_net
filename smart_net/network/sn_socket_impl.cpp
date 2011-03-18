/*
 * socket.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#include "sn_net_addr.h"
#include "sn_socket_impl.h"

#if __PLATFORM__ == __PLATFORM_LINUX__
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include <utils/utils.h>

namespace nm_network
{

/**
 * tcp socket.
 * */

CTcpSock::CTcpSock() :
	m_sockhandle(INVALID_SOCKET)
{
}

CTcpSock::~CTcpSock()
{
}

int32_t CTcpSock::open(sock_handle_t sockhandle)
{
	SYS_ASSERT(!is_opened());
	if (is_opened())
	{
		close();
	}

	if (INVALID_SOCKET < sockhandle)
	{
		m_sockhandle = sockhandle;
	}
	else
	{
		m_sockhandle = socket(AF_INET, SOCK_STREAM, 0);
	}

	return INVALID_SOCKET < m_sockhandle ? CMNERR_SUC : SNERR_CREAT_SOCK_FAILDED;
}

int32_t CTcpSock::close()
{
	if (INVALID_SOCKET < m_sockhandle)
	{
		::close(m_sockhandle);
		m_sockhandle = INVALID_SOCKET;
	}

	return CMNERR_SUC;
}

int32_t CTcpSock::set_nonblock(bool bFlag)
{
	return nm_utils::set_block_flag(m_sockhandle, !bFlag);
}

int32_t CTcpSock::bind(const CIpv4Addr &localAddr)
{
	SYS_ASSERT(INVALID_SOCKET < m_sockhandle);

	struct sockaddr_in bindAddr;
	bindAddr.sin_family = AF_INET;
	bindAddr.sin_port = localAddr.get_port_nbo();
	bindAddr.sin_addr
			= *(static_cast<struct in_addr*> (localAddr.get_ip_nbo()));

	return ::bind(m_sockhandle, (struct sockaddr*) &bindAddr, sizeof(bindAddr));
}

int32_t CTcpSock::listen(int32_t i32Backlog)
{
	return ::listen(m_sockhandle, i32Backlog);
}

tcp_sock_ptr_t CTcpSock::accept()
{
	tcp_sock_ptr_t ptcpsock = NULL;

	struct sockaddr_in remoteAddr;
	ZERO_MEM(&remoteAddr, sizeof(remoteAddr));
	socklen_t slSize = sizeof(remoteAddr);
	sock_handle_t sockhandle = ::accept(m_sockhandle,
			reinterpret_cast<struct sockaddr*> (&remoteAddr), &slSize);
	if (INVALID_SOCKET >= sockhandle)
	{
		SYS_ASSERT(false);
		return ptcpsock;
	}

	ptcpsock = SYS_NOTRW_NEW(CTcpSock);
	SYS_ASSERT(NULL != ptcpsock);
	ptcpsock->open(sockhandle);

	return ptcpsock;
}

int32_t CTcpSock::connect(const CIpv4Addr &remoteAddr)
{
	struct sockaddr_in destAddr;
	destAddr.sin_family = AF_INET;
	destAddr.sin_port = remoteAddr.get_port_nbo();
	destAddr.sin_addr = *static_cast<struct in_addr*> (remoteAddr.get_ip_nbo());

	int32_t i32ret = ::connect(m_sockhandle,
			reinterpret_cast<struct sockaddr*> (&destAddr), sizeof(destAddr));

	return CMNERR_SUC == i32ret ? CMNERR_SUC
			: (EINPROGRESS == errno ? SNERR_IN_PROGRESS : CMNERR_COMMON_ERR);
}

///**
// * udp ..
// * */
//CUdpSock::CUdpSock()
//{
//	// TODO Auto-generated constructor stub
//
//}
//
//CUdpSock::~CUdpSock()
//{
//	// TODO Auto-generated destructor stub
//}
//
//CRmpSock::CRmpSock()
//{
//	// TODO Auto-generated constructor stub
//
//}
//
//CRmpSock::~CRmpSock()
//{
//	// TODO Auto-generated destructor stub
//}

}
