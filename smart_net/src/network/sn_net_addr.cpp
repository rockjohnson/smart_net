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

CIpv4Addr::CIpv4Addr(const CIpv4Addr &other)
:m_ui16port(other.m_ui16port)
,m_ip(other.m_ip)
{}

CIpv4Addr& CIpv4Addr::operator =(const CIpv4Addr &other)
{
	m_ui16port = other.m_ui16port;
	m_ip = other.m_ip;

	return *this;
}

bool CIpv4Addr::is_valid()
{
	return 0 < m_ui16port;
}

int32_t CIpv4Addr::set_ip(cmn_cstr_t pcszIp)
{
	IF_TRUE_THEN_RETURN_CODE(NULL == pcszIp, CMNERR_COMMON_ERR);

	return ip_ston(AF_INET, pcszIp, static_cast<cmn_pvoid_t>(&m_ip));
}

int32_t CIpv4Addr::set_ip_nbo(cmn_pvoid_t pIp, u_int32_t ui32Len)
{
	IF_TRUE_THEN_RETURN_CODE(NULL == pIp || INET_ADDRSTRLEN != ui32Len, CMNERR_COMMON_ERR);

	memcpy(&m_ip, pIp, ui32Len);

	return CMNERR_SUC;
}

cmn_pvoid_t CIpv4Addr::get_ip_nbo() const
{
	return reinterpret_cast<cmn_pvoid_t>(const_cast<struct in_addr*>(&m_ip));
}

cmn_cstr_t CIpv4Addr::get_ip_str(cmn_char_t *pszBuf, u_int32_t ui32Len)
{
	IF_TRUE_THEN_RETURN_CODE(NULL == pszBuf, NULL);

	return ip_ntos(AF_INET, reinterpret_cast<cmn_pvoid_t>(&m_ip), pszBuf, ui32Len) == CMNERR_SUC ? pszBuf : NULL;
}

int32_t CIpv4Addr::set_port_nbo(u_int16_t ui16Port /*network byte order*/)
{
	IF_TRUE_THEN_RETURN_CODE(0 == ui16Port, CMNERR_COMMON_ERR);

	m_ui16port = ui16Port;

	return CMNERR_SUC;
}

int32_t CIpv4Addr::set_port_hbo(u_int16_t ui16Port /*host byte order*/)
{
	IF_TRUE_THEN_RETURN_CODE(0 == ui16Port, CMNERR_COMMON_ERR);

	m_ui16port = htons(ui16Port);

	return CMNERR_SUC;
}

u_int16_t CIpv4Addr::get_port_hbo() const
{
	return ntohs(m_ui16port);
}

u_int16_t CIpv4Addr::get_port_nbo() const
{
	return m_ui16port;
}


}
