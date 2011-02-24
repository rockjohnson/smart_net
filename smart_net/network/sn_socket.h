/*
 * socket.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include "../framework/sn_io_obj.h"
#include "sn_net_addr.h"

namespace nm_framework
{

/**
 * base socket class
 * */
class ISocket : public nm_base::ICommonBase
{
public:
	ISocket();
	virtual ~ISocket();

	enum {INVALID_SOCKET = -1};
public:
	virtual int32_t create(int32_t i32fd = INVALID_SOCKET) = 0;
	virtual int32_t close() = 0;
	virtual int32_t bind(INetAddr &localAddr) = 0;
	virtual int32_t listen(int32_t i32Backlog) = 0;
	virtual int32_t connect(INetAddr &remoteAddr) = 0;
	virtual int32_t get_fd() = 0;
	virtual bool is_valid() = 0;
	virtual int32_t set_nonblock(bool bFlag) = 0;
};
typedef nm_utils::CSmartPtr<nm_network::ISocket> sock_ptr_t;

/**
 * tcp socket
 *
 * */
class CTcpSock: public nm_framework::ISocket
{

public:
	CTcpSock();
	virtual ~CTcpSock();

public:
	int32_t create(int32_t i32fd = INVALID_SOCKET);
	int32_t close();
	int32_t bind(INetAddr &localAddr);
	int32_t listen(int32_t i32Backlog);
	tcp_sock_ptr_t accept();
	int32_t connect(INetAddr &remoteAddr);
	int32_t get_fd();
	bool is_valid();
	int32_t set_nonblock(bool bFlag);

private:
	int32_t m_i32fd;
};
typedef nm_utils::CSmartPtr<nm_network::CTcpSock> tcp_sock_ptr_t;


/**
 * udp socket
 * */
class CUdpSock: public nm_network::ISocket
{
public:
	CUdpSock();
	virtual ~CUdpSock();
};


/**
 * rmp(reliable multicast protocal) socket
 * */
class CRmpSock: public nm_network::ISocket
{
public:
	CRmpSock();
	virtual ~CRmpSock();
};

}

#endif /* SOCKET_H_ */
