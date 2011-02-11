/*
 * conn.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef CONN_H_
#define CONN_H_

#include "socket.h"

namespace nm_net
{

/**
 * base connection class
 * */
class CConn
{
public:
	CConn();
	virtual ~CConn();

public:
	virtual int init() = 0;
	virtual int send_data(mem_ptr_t &pData) = 0;

private:
	virtual void on_connected() = 0;
	virtual void on_recv_data(mem_ptr_t &pData) = 0;
	virtual void on_closed(int32_t iErrCode) = 0;

};

/**
 * tcp connection
 * */
class CTcpConn: public nm_net::CConn
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
class CUdpConn: public nm_net::CConn
{
public:
	CUdpConn();
	virtual ~CUdpConn();
};

/**
 *
 * rmp connection
 * */
class CRmpConn: public nm_net::CConn
{
public:
	CRmpConn();
	virtual ~CRmpConn();
};

}

#endif /* CONN_H_ */
