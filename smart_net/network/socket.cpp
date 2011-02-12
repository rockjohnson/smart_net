/*
 * socket.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#include "socket.h"
#if __PLATFORM__ == __PLATFORM_LINUX__
#include <sys/types.h>
#include <sys/socket.h>
#endif


namespace nm_network
{

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

int32_t CTcpSock::init(int32_t i32fd)
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
