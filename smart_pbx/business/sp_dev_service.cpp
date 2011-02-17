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
