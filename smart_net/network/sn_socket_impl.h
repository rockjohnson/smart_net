/*
 * socket.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include "../common/sn_common.h"
#include "../framework/sn_socket.h"
#include "sn_net_addr.h"

namespace nm_network
{



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
	virtual int32_t open(int32_t i32fd = INVALID_SOCKET){}
	virtual int32_t close(){}
	virtual int32_t bind(const CIpv4Addr &bindaddr){}
	virtual int32_t listen(int32_t i32Backlog){}
	virtual int32_t connect(const CIpv4Addr &remoteAddr){}
	virtual sock_handle_t get_fd(){}
	virtual bool is_opened(){}
	virtual int32_t set_nonblock(bool bFlag){}
	virtual CIpv4Addr& get_peer_addr(){}
	virtual CIpv4Addr& get_local_addr(){}

private:
	int32_t m_i32fd;
};
typedef nm_utils::CSmartPtr<nm_network::CTcpSock> tcp_sock_ptr_t;


///**
// * udp socket
// * */
//class CUdpSock: public nm_network::ISocket
//{
//public:
//	CUdpSock();
//	virtual ~CUdpSock();
//};
//
//
///**
// * rmp(reliable multicast protocal) socket
// * */
//class CRmpSock: public nm_network::ISocket
//{
//public:
//	CRmpSock();
//	virtual ~CRmpSock();
//};

}

#endif /* SOCKET_H_ */
