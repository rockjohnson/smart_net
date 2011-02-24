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
