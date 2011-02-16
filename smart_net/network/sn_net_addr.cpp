/*
 * net_addr.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 */

#include "sn_net_addr.h"

namespace nm_network
{

CNetAddr::CNetAddr()
{
	// TODO Auto-generated constructor stub

}

CNetAddr::~CNetAddr()
{
	// TODO Auto-generated destructor stub
}

/**
 * valid
 * */
bool CIpv4Addr::is_valid()
{
	return m_ui16Port > 0;
}

}
