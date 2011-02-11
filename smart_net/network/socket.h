/*
 * socket.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include "../framework/io_obj.h"

namespace nm_net
{

/**
 * base socket class
 * */
class CSocket: public nm_framework::CIoObj
{
public:
	CSocket();
	virtual ~CSocket();
};

typedef nm_utils::CSmartPtr<nm_net::CSocket> sock_ptr_t;

/**
 * tcp socket
 *
 * */
class CTcpSock: public nm_net::CSocket
{
public:
	CTcpSock();
	virtual ~CTcpSock();
};


/**
 * udp socket
 * */
class CUdpSock: public nm_net::CSocket
{
public:
	CUdpSock();
	virtual ~CUdpSock();
};


/**
 * rmp(reliable multicast protocal) socket
 * */
class CRmpSock: public nm_net::CSocket
{
public:
	CRmpSock();
	virtual ~CRmpSock();
};

}

#endif /* SOCKET_H_ */
