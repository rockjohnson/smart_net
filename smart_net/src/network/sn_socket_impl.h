/*
 * socket.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef __SN_SOCKET_IMPL_H__
#define __SN_SOCKET_IMPL_H__

#include <memory/mem.h>

#include "../common/sn_common.h"
#include "sn_socket.h"

namespace nm_network
{

/**
 * tcp socket
 *
 * */
class CTcpSock;
typedef nm_utils::CSmartPtr<nm_network::CTcpSock> tcp_sock_ptr_t;
class CTcpSock: public nm_framework::ISocket
{

public:
	CTcpSock();
	virtual ~CTcpSock();

public:
	virtual int32_t open(sock_handle_t sockhandle = INVALID_SOCKET);
	virtual int32_t close();
	virtual int32_t bind(CIpv4Addr &bindaddr);
	virtual int32_t bind(const std::string &strBindIP, u_int16_t ui16BindPort);
	virtual int32_t listen(int32_t i32Backlog);
	virtual tcp_sock_ptr_t accept();
	virtual int32_t connect(const CIpv4Addr &remoteAddr);
	virtual int32_t connect(const cmn_string_t &strAcceptorIp, u_int64_t ui16AcceptorPort);
	virtual sock_handle_t get_handle();
	virtual bool is_opened();
	virtual int32_t set_nonblock(bool bFlag);
	virtual CIpv4Addr& get_peer_addr();
	virtual CIpv4Addr& get_local_addr();

	virtual int32_t send(nm_mem::mem_ptr_t&);
	virtual int32_t send(cmn_pvoid_t pV, u_int32_t ui32Len);

	virtual int32_t recv(nm_mem::mem_ptr_t&);
	virtual int32_t recv(cmn_pvoid_t pV, u_int32_t ui32Size);

private:
	CIpv4Addr m_localaddr;
	CIpv4Addr m_peeraddr;
	sock_handle_t m_handle;
};


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
