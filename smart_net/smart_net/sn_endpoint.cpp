/*
 * conn.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#include "sn_endpoint.h"

namespace nm_smartnet
{

IEndpoint::IEndpoint()
{
	// TODO Auto-generated constructor stub

}

IEndpoint::~IEndpoint()
{
	destroy();
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

}


CUdpEndpoint::CUdpEndpoint()
{
	// TODO Auto-generated constructor stub

}

CUdpEndpoint::~CUdpEndpoint()
{
	// TODO Auto-generated destructor stub
}


CRmpEndpoint::CRmpEndpoint()
{
	// TODO Auto-generated constructor stub

}

CRmpEndpoint::~CRmpEndpoint()
{
	// TODO Auto-generated destructor stub
}

}
