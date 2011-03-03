/*
 * conn.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#include "sn_endpoint.h"

namespace nm_network
{

/**
 * tcp endpoint.
 * */
CTcpInboundEndpoint::CTcpInboundEndpoint(smart_net_ptr_t &pSmartNet)
:m_pSmartNetMgr(NULL == pSmartNet ? NULL : pSmartNet->get_mgr())
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
int32_t CTcpInboundEndpoint::open(const net_addr_ptr_t &pListenAddr, const net_addr_ptr_t &pPeerAddr)
{
	SYS_ASSERT(NULL != m_pSmartNetMgr);
	m_pListenAddr = pListenAddr;
	m_pPeerAddr = pPeerAddr;

	return m_pSmartNetMgr->add_endpoint(endpoint_ptr_t(this));
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
