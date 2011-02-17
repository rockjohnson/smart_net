/*
 * net_addr.h
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 */

#ifndef NET_ADDR_H_
#define NET_ADDR_H_

#include <arpa/inet.h>

namespace nm_network
{

/**
 * net address.
 * */
class INetAddr
{
public:
	INetAddr();
	virtual ~INetAddr();

public:
	virtual int32_t set_ip(cstr_t pcszIp) = 0;
	virtual int32_t set_ip_nbo(pvoid_t pIp, u_int32_t ui32Len) = 0;
	virtual pvoid_t get_ip_nbo() = 0;
	virtual cstr_t get_ip_str(char_t *pszBuf, u_int32_t ui32Len) = 0;
	///
	virtual int32_t set_port_nbo(u_int16_t ui16Port /*network byte order*/) = 0;
	virtual int32_t set_port_hbo(u_int16_t ui16Port /*host byte order*/) = 0;
	virtual u_int16_t get_port_hbo() = 0;
	virtual u_int16_t get_port_nbo() = 0;
	///
	virtual bool is_valid() = 0;
};

/**
 * ipv4 address.
 * */
class CIpv4Addr : public INetAddr
{
public:
	CIpv4Addr();
	virtual ~CIpv4Addr();

public:
	virtual int32_t set_ip(cstr_t pcszIp);
	virtual int32_t set_ip_nbo(pvoid_t pIp, u_int32_t ui32Len);
	virtual pvoid_t get_ip_nbo();
	virtual cstr_t get_ip_str(char_t *pszBuf, u_int32_t ui32Len);
	///
	virtual int32_t set_port_nbo(u_int16_t ui16Port /*network byte order*/);
	virtual int32_t set_port_hbo(u_int16_t ui16Port /*host byte order*/);
	virtual u_int16_t get_port_hbo();
	virtual u_int16_t get_port_nbo();
	///
	virtual bool is_valid();

private:
	u_int16_t m_ui16Port; ///network byte order.
	struct in_addr m_inAddr; ///network byte order.
};

}

#endif /* NET_ADDR_H_ */
