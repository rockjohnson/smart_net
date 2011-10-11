/*
 * net_addr.h
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 */

#ifndef NET_ADDR_H_
#define NET_ADDR_H_

#include <arpa/inet.h>
#include <common/common.h>
#include <utils/smart_ptr.h>

namespace nm_network
{

/**
 * net address.
 * */
class INetAddr : public nm_cmn_base::ICommonBase
{
public:
	INetAddr();
	virtual ~INetAddr();

public:
	virtual int32_t set_ip(cmn_cstr_t pcszIp) = 0;
	virtual int32_t set_ip_nbo(cmn_pvoid_t pIp, u_int32_t ui32Len) = 0;
	virtual cmn_pvoid_t get_ip_nbo() const = 0;
	virtual cmn_cstr_t get_ip_str(cmn_char_t *pszBuf, u_int32_t ui32Len) = 0;
	///
	virtual int32_t set_port_nbo(u_int16_t ui16Port /*network byte order*/) = 0;
	virtual int32_t set_port_hbo(u_int16_t ui16Port /*host byte order*/) = 0;
	virtual u_int16_t get_port_hbo() const = 0;
	virtual u_int16_t get_port_nbo() const = 0;
	///
	virtual bool is_valid() = 0;
};
typedef nm_utils::CSmartPtr<nm_network::INetAddr> net_addr_ptr_t;

/**
 * ipv4 address.
 * */
class CIpv4Addr : public INetAddr
{
public:
	CIpv4Addr();
	virtual ~CIpv4Addr();

	CIpv4Addr(const CIpv4Addr &other);
	CIpv4Addr& operator =(const CIpv4Addr &other);

public:
	virtual int32_t set_ip(cmn_cstr_t pcszIp);
	virtual int32_t set_ip_nbo(cmn_pvoid_t pIp, u_int32_t ui32Len);
	virtual cmn_pvoid_t get_ip_nbo() const;
	virtual cmn_cstr_t get_ip_str(cmn_char_t *pszBuf, u_int32_t ui32Len);
	///
	virtual int32_t set_port_nbo(u_int16_t ui16Port /*network byte order*/);
	virtual int32_t set_port_hbo(u_int16_t ui16Port /*host byte order*/);
	virtual u_int16_t get_port_hbo() const;
	virtual u_int16_t get_port_nbo() const;
	///
	virtual bool is_valid();

private:
	u_int16_t m_ui16port; ///network byte order.
	struct in_addr m_ip; ///network byte order.
};
typedef nm_utils::CSmartPtr<nm_network::CIpv4Addr> ipv4_addr_ptr_t;


}

#endif /* NET_ADDR_H_ */
