/*
 * net_addr.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 */

#include "sn_net_addr.h"
#include <utils/utils.h>
#include <string.h>


namespace nm_network
{

using namespace nm_utils;

INetAddr::INetAddr()
{
	// TODO Auto-generated constructor stub

}

INetAddr::~INetAddr()
{
	// TODO Auto-generated destructor stub
}

/**
 * ip v4 address.
 * */
bool CIpv4Addr::is_valid()
{
	return 0 < m_ui16Port;
}

int32_t CIpv4Addr::set_ip(cstr_t pcszIp)
{
	IF_TRUE_THEN_RETURN_CODE(NULL == pcszIp, CMNERR_COMMON_ERR);

	return ip_ston(AF_INET, pcszIp, static_cast<pvoid_t>(&m_inAddr));
}

int32_t CIpv4Addr::set_ip_nbo(pvoid_t pIp, u_int32_t ui32Len)
{
	IF_TRUE_THEN_RETURN_CODE(NULL == pIp || INET_ADDRSTRLEN != ui32Len, CMNERR_COMMON_ERR);

	memcpy(&m_inAddr, pIp, ui32Len);

	return CMNERR_SUC;
}

pvoid_t CIpv4Addr::get_ip_nbo()
{
	return &m_inAddr;
}

cstr_t CIpv4Addr::get_ip_str(char_t *pszBuf, u_int32_t ui32Len)
{
	IF_TRUE_THEN_RETURN_CODE(NULL == pszBuf, NULL);

	return ip_ntos(AF_INET, static_cast<pvoid_t>(&m_inAddr), pszBuf, ui32Len) == CMNERR_SUC ? pszBuf : NULL;
}

int32_t CIpv4Addr::set_port_nbo(u_int16_t ui16Port /*network byte order*/)
{
	IF_TRUE_THEN_RETURN_CODE(0 == ui16Port, CMNERR_COMMON_ERR);

	m_ui16Port = ui16Port;

	return CMNERR_SUC;
}

int32_t CIpv4Addr::set_port_hbo(u_int16_t ui16Port /*host byte order*/)
{
	IF_TRUE_THEN_RETURN_CODE(0 == ui16Port, CMNERR_COMMON_ERR);

	m_ui16Port = htons(ui16Port);

	return CMNERR_SUC;
}

u_int16_t CIpv4Addr::get_port_hbo()
{
	return ntohs(m_ui16Port);
}

u_int16_t CIpv4Addr::get_port_nbo()
{
	return m_ui16Port;
}


}
