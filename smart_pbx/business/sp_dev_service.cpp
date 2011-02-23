/*
 * dev_service.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: rock
 */

#include "sp_dev_service.h"

namespace nm_business
{

/**
 * CDevEndpoint
 * */
CDevEndpoint::CDevEndpoint()
{

}

CDevEndpoint::~CDevEndpoint()
{
}


/**
 * factory
 * */
CDevEndpointFactory::CDevEndpointFactory()
{}

CDevEndpointFactory::~CDevEndpointFactory()
{}

tcp_endpoint_factory_t::obj_ptr_t& CDevEndpointFactory::create_obj(int32_t i32Type)
{
	tcp_endpoint_factory_t::obj_ptr_t pObj(NULL);

	switch (i32Type)
	{
	case CTcpEndpoint::TCP_ENDPOINT:
	{
		pObj = SYS_NOTRW_NEW(CDevEndpoint);
		break;
	}
	case CTcpOutboundEndpoint::TCP_OUTBOUND_ENDPOINT:
	{
		break;
	}
	default:
	{
		break;
	}
	}

	return pObj;
}


/**
 * device service
 *
 * */
CDevService::CDevService(CSmartNet &smartnet)
:CTcpService(smartnet)
{
	// TODO Auto-generated constructor stub

}

CDevService::~CDevService()
{
	// TODO Auto-generated destructor stub
}

}
