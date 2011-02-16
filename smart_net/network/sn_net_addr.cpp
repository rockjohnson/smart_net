/*
 * net_addr.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 */

#include "sn_net_addr.h"
#include <arpa/inet.h>


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
 * ip v4 address.
 * */
bool CIpv4Addr::is_valid()
{
	return m_ui16Port > 0;
}

int32_t CIpv4Addr::set_ip(cstr_t pcszIp)
{
    struct in_addr addr;

    if (inet_aton(pcszIp, &addr) == 0)
    {
        return CMNERR_COMMON_ERR;
    }

    m_inAddr = addr.s_addr;
}

}
