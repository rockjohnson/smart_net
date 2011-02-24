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

INetService::INetService(net_engine_ptr_t &pNetEngine)
:m_pNetEngine(pNetEngine)
{
	// TODO Auto-generated constructor stub
}

INetService::~INetService()
{
	stop();
}

int32_t INetService::start(net_addr_ptr_t &pLocalNetAddr, net_addr_ptr_t &pPeerNetAddr)
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
#define TCP_SERV_TEMPLATE_DEFINE(func, ret) \
	ret CTcpService::func

TCP_SERV_TEMPLATE_DEFINE(CTcpService(CSmartNet &smartnet),)
:INetService(smartnet.get_net_engine())
{
}

TCP_SERV_TEMPLATE_DEFINE(~CTcpService(),)
{
}

TCP_SERV_TEMPLATE_DEFINE(start(net_addr_ptr_t &pLocalNetAddr, net_addr_ptr_t &pRemoteAddr, endpoint_factory_ptr_t &pEndpointFactory), int32_t)
{
	IF_TRUE_THEN_RETURN_CODE((NULL != pLocalNetAddr && NULL != pRemoteAddr) || (NULL == pLocalNetAddr && NULL == pRemoteAddr) || (NULL == pEndpointFactory), CMNERR_COMMON_ERR);

	m_pEndpointFactory = pEndpointFactory;

	INetService::start(pLocalNetAddr, pRemoteAddr);

	if (NULL != pLocalNetAddr)
	{
		start_listen_service(pLocalNetAddr, m_i32Backlog);
	}
	else if (NULL != pRemoteAddr)
	{
		start_connect_service(pRemoteAddr, m_i32ConnTimeout, m_i32MaxRetries);
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

TCP_SERV_TEMPLATE_DEFINE(set_listen_serv_info(int32_t i32Backlog), void)
{
	m_i32Backlog = i32Backlog;
}

TCP_SERV_TEMPLATE_DEFINE(start_listen_service(net_addr_ptr_t &pBindAddr, int32_t i32Backlog), int32_t)
{
	///
	m_pTcpListener = SYS_NOTRW_NEW(CTcpListener);
	SYS_ASSERT(NULL != m_pTcpListener);
	///
	IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC > m_pTcpListener->start(pBindAddr, i32Backlog, m_pNetEngine), CMNERR_COMMON_ERR);

	return CMNERR_SUC;
}

TCP_SERV_TEMPLATE_DEFINE(start_connect_service(net_addr_ptr_t &pRemoteAddr, int32_t i32ConnTimeout, int32_t i32MaxRetries), int32_t)
{
	m_pOutboundEndpoint = dynamic_cast_smartptr<CTcpOutboundEndpoint, CTcpEndpoint>(m_pEndpointFactory->create_obj(E_TCP_OUTBOUND_ENDPOINT));
	if (NULL == m_pOutboundEndpoint)
	{
		return CMNERR_COMMON_ERR;
	}

	return m_pOutboundEndpoint->start(pRemoteAddr, i32ConnTimeout, i32MaxRetries);
}

}
