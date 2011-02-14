/*
 * net_addr.h
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 */

#ifndef NET_ADDR_H_
#define NET_ADDR_H_

namespace nm_network
{

class INetAddr
{
public:
	INetAddr();
	virtual ~INetAddr();

public:
	u_int16_t get_port() = 0;
	u_int32_t get_ip() = 0;
	bool is_valid() = 0;
};

class CIpv4Addr : public INetAddr
{
public:
	CIpv4Addr();
	virtual ~CIpv4Addr();

public:
	u_int16_t get_port();
	u_int32_t get_ip();

private:
	u_int16_t m_ui16Port;
	u_int32_t m_ui32Ip;
};

}

#endif /* NET_ADDR_H_ */
