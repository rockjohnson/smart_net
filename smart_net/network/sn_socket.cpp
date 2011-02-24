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
