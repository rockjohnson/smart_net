/*
 * conn.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef CONN_H_
#define CONN_H_

#include "../network/socket.h"

namespace nm_smartnet
{

/**
 * base endpoint class
 * */
class IEndpoint
{
public:
	IEndpoint();
	virtual ~IEndpoint();

public:
	virtual int32_t init() = 0;
	virtual int32_t close() = 0;
	virtual int32_t send_data(mem_ptr_t &pData) = 0;

protected:
	virtual void on_connected() = 0;
	virtual void on_recv_data(mem_ptr_t &pData) = 0;
	virtual void on_closed(int32_t iErrCode) = 0;
};

/**
 * tcp endpoint
 * */
class CTcpEndpoint: public nm_network::IEndpoint
{
public:
	CTcpEndpoint();
	virtual ~CTcpEndpoint();

public:
	virtual int send_data(mem_ptr_t &pData);

protected:
	virtual void on_connected();
	virtual void on_recv_data(mem_ptr_t &pData);
	virtual void on_closed(int32_t iErrCode);

private:
	tcp_sock_ptr_t m_pSock;
};

/**
 * udp endpoint
 * */
class CUdpEndpoint: public nm_network::IEndpoint
{
public:
	CUdpEndpoint();
	virtual ~CUdpEndpoint();
};

/**
 * rmp endpoint
 * */
class CRmpEndpoint: public nm_network::IEndpoint
{
public:
	CRmpEndpoint();
	virtual ~CRmpEndpoint();
};

}

#endif /* CONN_H_ */
