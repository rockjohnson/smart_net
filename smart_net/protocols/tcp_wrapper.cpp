/*
 * tcp_wrapper.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: rock
 */

#include "tcp_wrapper.h"

namespace nm_protocol
{

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
		close();
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
		::close(m_i32fd);
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
	bindAddr.sin_port = localAddr.get_port_nbo();
	bindAddr.sin_addr = *(static_cast<struct in_addr*>(localAddr.get_ip_nbo()));

	return ::bind(m_i32fd, (struct sockaddr*)&bindAddr, sizeof(bindAddr));
}

int32_t CTcpSock::listen(int32_t i32Backlog)
{
	return ::listen(m_i32fd, i32Backlog);
}

CTcpSock::tcp_sock_ptr_t CTcpSock::accept()
{
	tcp_sock_ptr_t pTcpSock;
	struct sockaddr_in remoteAddr;
	ZERO_MEM(&remoteAddr, sizeof(remoteAddr));
	socklen_t slSize = sizeof(remoteAddr);
	int32_t i32NewSockFd = ::accept(m_i32fd, reinterpret_cast<struct sockaddr*>(&remoteAddr), &slSize);
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
	destAddr.sin_port = remoteAddr.get_port_nbo();
	destAddr.sin_addr = *static_cast<struct in_addr*>(remoteAddr.get_ip_nbo());

	int32_t i32Ret = ::connect(m_i32fd, reinterpret_cast<struct sockaddr*>(&destAddr), sizeof(destAddr));

	return CMNERR_SUC == i32Ret ? CMNERR_SUC : (EINPROGRESS == errno ? SNERR_IN_PROGRESS : CMNERR_COMMON_ERR);
}

/**
 * tcp endpoint.
 * */
CTcpEndpoint::CTcpEndpoint()
{
	// TODO Auto-generated constructor stub

}

CTcpEndpoint::~CTcpEndpoint()
{
	// TODO Auto-generated destructor stub
}

/**
 * outbound
 * */
CTcpOutboundEndpoint::CTcpOutboundEndpoint()
{}

CTcpOutboundEndpoint::~CTcpOutboundEndpoint()
{}

int32_t CTcpOutboundEndpoint::start(net_addr_ptr_t &pRemoteAddr, int32_t i32ConnTimeout, int32_t i32MaxRetries)
{
	m_pConnecter = SYS_NOTRW_NEW(CTcpConnecter);
}

/**
 * CTcpListener
 * */

int32_t CTcpListener::start(net_addr_ptr_t &pBindAddr, int32_t i32Backlog, net_engine_ptr_t &pNetEngine)
{
	///create listen socket.
	int32_t i32Ret = m_tcpSock.create();
	IF_TRUE_THEN_RETURN_CODE(0 > i32Ret, -1);

	i32Ret = m_tcpSock.bind(*pBindAddr);
	IF_TRUE_THEN_RETURN_CODE(0 > i32Ret, -2);

	i32Ret = m_tcpSock.listen(i32Backlog);
	IF_TRUE_THEN_RETURN_CODE(0 > i32Ret, -3);

	m_pNetEngine = pNetEngine;
	IF_TRUE_THEN_RETURN_CODE(NULL == m_pNetEngine, -4);

	i32Ret = m_pNetEngine->add_io_obj(io_obj_ptr_t(this));
	if (CMNERR_SUC > i32Ret)
	{
		stop();
	}

	return i32Ret;
}

int32_t CTcpListener::get_fd()
{
	return m_tcpSock.get_fd();
}

}
