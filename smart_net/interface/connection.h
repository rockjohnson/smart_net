/*
 * conn.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef CONN_H_
#define CONN_H_

#include "../network/socket.h"

namespace nm_network
{

/**
 * base connection class
 * */
class CConnection
{
public:
	CConnection();
	virtual ~CConnection();

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
 * tcp connection
 * */
class CTcpConn: public nm_network::CConnection
{
public:
	CTcpConn();
	virtual ~CTcpConn();

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
 * udp connection
 * */
class CUdpConn: public nm_network::CConnection
{
public:
	CUdpConn();
	virtual ~CUdpConn();
};

/**
 * rmp connection
 * */
class CRmpConn: public nm_network::CConnection
{
public:
	CRmpConn();
	virtual ~CRmpConn();
};

}

#endif /* CONN_H_ */
