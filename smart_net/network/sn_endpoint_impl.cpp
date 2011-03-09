/*
 * conn.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#include "sn_endpoint_impl.h"

namespace nm_network
{

/**
 * tcp endpoint.
 * */
CTcpInboundEndpoint::CTcpInboundEndpoint(smart_net_ptr_t &pSmartNet)
:m_psmartnetmgr(NULL == pSmartNet ? NULL : pSmartNet->get_mgr())
{
	// TODO Auto-generated constructor stub
}

CTcpInboundEndpoint::~CTcpInboundEndpoint()
{
	// TODO Auto-generated destructor stub
}

/**
 * open this endpoint and put it into the io engine.
 * */
int32_t CTcpInboundEndpoint::open(const net_addr_ptr_t &pListenAddr, const net_addr_ptr_t &pPeerAddr, const nm_framework::smart_net_mgr_ptr_t psmartnetmgr)
{
	nm_utils::mtx_scopelk_t lk(m_lkendpoint);

	if (NULL != m_psmartnetmgr)
	{
		return CMNERR_COMMON_ERR;
	}

	m_psock = SYS_NOTRW_NEW(CTcpSock);
	m_plistenaddr = pListenAddr;
	m_ppeeraddr = pPeerAddr;

	return m_psmartnetmgr->add_endpoint(endpoint_ptr_t(this));
}

/**
 * close this endpoint.
 * */
int32_t CTcpInboundEndpoint::close()
{
	nm_utils::mtx_scopelk_t lk(m_lkendpoint);

	if (NULL == m_psmartnetmgr)
	{
		return CMNERR_COMMON_ERR;
	}

	///first, delete it from engine.
	int32_t i32ret = m_psmartnetmgr->del_endpoint(nm_framework::endpoint_ptr_t(this));
	SYS_ASSERT(CMNERR_SUC == i32ret);
	m_psmartnetmgr = NULL;

	///then, close the socket.
	SYS_ASSERT(NULL != m_psock);
	m_psock->close();
	m_psock = NULL;
	m_plistenaddr = NULL;
	m_ppeeraddr = NULL;

	return CMNERR_SUC;
}

int32_t CTcpInboundEndpoint::get_type()
{
	return E_TCP_INBOUND_ENDPOINT;
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

}
