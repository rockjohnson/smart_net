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

INetService::INetService(net_engine_ptr_t &pNetEngine)
:m_pNetEngine(pNetEngine)
{
	// TODO Auto-generated constructor stub
}

INetService::~INetService()
{
	stop();
}

int32_t INetService::start(net_addr_ptr_t &pLocalNetAddr, net_addr_ptr_t pPeerNetAddr)
{
	m_pLocalNetAddr = pLocalNetAddr;
	m_pPeereNetAddr = pPeerNetAddr;

	return CMNERR_SUC;
}

//int32_t INetService::start(net_addr_ptr_t &pLocalNetAddr, net_addr_ptr_t &pPeerNetAddr)
//{
//	m_pLocalNetAddr = pLocalNetAddr;
//	m_pPeereNetAddr = pPeerNetAddr;
//
//	return CMNERR_SUC;
//}


/**
 * CTcpService
 * */
CTcpService::CTcpService(CSmartNet &smartnet)
:INetService(smartnet.get_net_engine())
{
}

CTcpService::~CTcpService()
{
}

int32_t CTcpService::start(net_addr_ptr_t &pLocalNetAddr, net_addr_ptr_t &pPeerNetAddr)
{
	IF_TRUE_THEN_RETURN_CODE((NULL != pLocalNetAddr && NULL != pPeerNetAddr) || (NULL == pLocalNetAddr && NULL == pPeerNetAddr), CMNERR_COMMON_ERR);

	INetService::start(pLocalNetAddr, pPeerNetAddr);

	if (NULL != pLocalNetAddr)
	{
		start_listen_service();
	}
	else if (NULL != pPeerNetAddr)
	{
		start_connect_service();
	}
	else
	{
		SYS_ASSERT(false);
	}

	return CMNERR_SUC;
}

//int32_t CTcpService::start()
//{
//	int32_t i32Ret = CMNERR_SUC;
//
//	if (m_localAddr.is_valid())
//	{
//		i32Ret = start_listen_service();
//	}
//	else if (m_remoteAddr.is_valid())
//	{
//		i32Ret = start_connect_service();
//	}
//	else
//	{
//		i32Ret = CMNER_NO_MATCH_ITEM;
//	}
//
//	return i32Ret;
//}

void CTcpService::set_backlog(int32_t i32Backlog)
{
	m_i32Backlog = i32Backlog;
}

int32_t CTcpService::start_listen_service()
{
	tcp_listener_ptr_t pTcpListener = SYS_NOTRW_NEW(LISTENER);
	int32_t i32Ret = pTcpListener->init(listenAddr, i32Backlog);
	IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC > i32Ret, CMNERR_COMMON_ERR);
	///
	i32Ret = m_netEngine.add_io_obj(pTcpListener);
	IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC > i32Ret, CMNERR_COMMON_ERR);
	///
	m_vecTcpListener.pushback(pTcpListener);

	return CMNERR_SUC;
}

int32_t CTcpService::start_connect_service()
{
	SYS_ASSERT(NULL == m_pTcpConnecter);

	m_pTcpConnecter = SYS_NOTRW_NEW(CONN);
	int32_t i32Ret = m_pTcpConnecter->init(m_remoteAddr, m_);
}




}
