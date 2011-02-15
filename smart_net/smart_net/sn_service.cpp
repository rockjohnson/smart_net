/*
 * service.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 */

#include "sn_service.h"
#include "../network/sn_socket.h"

namespace nm_smartnet
{

using namespace nm_network;

CService::CService()
{
	// TODO Auto-generated constructor stub
}

CService::~CService()
{
	destroy();
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

void CTcpService::set_backlog(int32_t i32Backlog)
{
	m_i32Backlog = i32Backlog;
}

template <class CONN>
int32_t CTcpService::start_listen_service()
{
	SYS_ASSERT(NULL == m_pTcpListener);

	///
	m_pTcpListener = SYS_NOTRW_NEW(CTcpListener<CONN>);
	int32_t i32Ret = m_pTcpListener->init(m_localAddr, m_i32Backlog);
	IF_TRUE_THEN_RETURN_CODE(0 > i32Ret, -1);

	///
	i32Ret = m_netEngine.add_io_obj(m_pTcpListener);
	IF_TRUE_THEN_RETURN_CODE(0 > i32Ret, -2);

}

template <class CONN>
int32_t CTcpService<CONN>::start_connect_service()
{
	SYS_ASSERT(NULL == m_pTcpConnecter);

	m_pTcpConnecter = SYS_NOTRW_NEW(CONN);
	int32_t i32Ret = m_pTcpConnecter->init(m_remoteAddr, m_);
}




}
