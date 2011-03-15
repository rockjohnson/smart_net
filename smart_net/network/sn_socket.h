/*
 * socket.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include "../common/sn_common.h"
#include "sn_net_addr.h"

namespace nm_network
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
	virtual int32_t open(int32_t i32fd = INVALID_SOCKET) = 0;
	virtual int32_t close() = 0;
	virtual int32_t bind(const CIpv4Addr &bindaddr) = 0;
	virtual int32_t listen(int32_t i32Backlog) = 0;
	virtual int32_t connect(const CIpv4Addr &remoteAddr) = 0;
	virtual sock_handle_t get_fd() = 0;
	virtual bool is_opened() = 0;
	virtual int32_t set_nonblock(bool bFlag) = 0;
};
typedef nm_utils::CSmartPtr<nm_network::ISocket> sock_ptr_t;

/**
 * tcp socket
 *
 * */
class CTcpSock: public ISocket
{

public:
	CTcpSock();
	virtual ~CTcpSock();

public:
	int32_t open(int32_t i32fd = INVALID_SOCKET);
	int32_t close();
	int32_t bind(const CIpv4Addr &localAddr);
	int32_t listen(int32_t i32Backlog);
	sock_ptr_t accept();
	int32_t connect(const CIpv4Addr &remoteAddr);
	sock_handle_t get_fd();
	bool is_opened();
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
