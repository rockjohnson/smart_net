/*
 * service.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 */

#include "service.h"
#include "../network/socket.h"

namespace nm_smartnet
{

using namespace nm_network;

CService::CService()
{
	// TODO Auto-generated constructor stub

}

CService::~CService()
{
	// TODO Auto-generated destructor stub
}


/**
 * CTcpService
 * */
int32_t CTcpService::init(INetAddr &localAddr, INetAddr &remoteAddr, int32_t i32MaxInboundConnectionCnt, int32_t i32MinOutboundConnectionCnt)
{
	if (localAddr.is_valid() == remoteAddr.is_valid()
			|| i32MaxInboundConnectionCnt < NO_CONNECTIONS_LIMIT
			|| i32MinOutboundConnectionCnt < NO_CONNECTIONS_LIMIT)
	{
		return CMNER_INVALID_PARAMS;
	}

	m_localAddr = localAddr;
	m_remoteAddr = remoteAddr;

	m_i32MaxInboundConnCnt = i32MaxInboundConnectionCnt;
	m_i32MinOutboundConnCnt = i32MinOutboundConnectionCnt;

	return CMNERR_SUC;
}

int32_t CTcpService::start()
{
	int32_t i32Ret = CMNERR_SUC;

	if (m_localAddr.is_valid())
	{
		i32Ret = start_listen_service();
	}
	else if (m_remoteAddr.is_valid())
	{
		i32Ret = start_connect_service();
	}
	else
	{
		i32Ret = CMNER_NO_MATCH_ITEM;
	}

	return i32Ret;
}

int32_t CTcpService::start_listen_service()
{
	///create a socket, and bind to the local addr, then listening.
	CTcpListener tcpListener;


}




}
