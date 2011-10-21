/*
 * sn_socket.h
 *
 *  Created on: Mar 18, 2011
 *      Author: rock
 */

#ifndef __SN_SOCKET_H__
#define __SN_SOCKET_H__

#include "../common/sn_common.h"
#include "../network/sn_net_addr.h"

namespace nm_framework
{

/**
 * base socket class
 * */
class ISocket: public nm_cmn_base::ICommonBase
{
public:
	ISocket()
	{
	}
	virtual ~ISocket()
	{
	}

	enum
	{
		INVALID_SOCKET = -1
	};

public:
	virtual int32_t open(sock_handle_t sockhandle) = 0;
	virtual int32_t close() = 0;
	virtual int32_t bind(nm_network::CIpv4Addr &bindaddr) = 0;
	virtual int32_t bind(const std::string &strBindIP, u_int16_t ui16BindPort) = 0;
	virtual int32_t listen(int32_t i32Backlog) = 0;
	virtual int32_t connect(const nm_network::CIpv4Addr &remoteAddr) = 0;
	virtual int32_t connect(const cmn_string_t &strAcceptorIp, u_int64_t ui16AcceptorPort) = 0;
	virtual sock_handle_t get_handle() = 0;
	virtual bool is_opened() = 0;
	virtual int32_t set_nonblock(bool bFlag) = 0;
	virtual nm_network::CIpv4Addr& get_peer_addr() = 0;
	virtual nm_network::CIpv4Addr& get_local_addr() = 0;

	virtual int32_t send(nm_mem::mem_ptr_t&) = 0;
	virtual int32_t send(cmn_pvoid_t pV, u_int32_t ui32Len) = 0;

	virtual int32_t recv(nm_mem::mem_ptr_t&) = 0;
	virtual int32_t recv(cmn_pvoid_t pV, u_int32_t ui32Size) = 0;
};
typedef nm_utils::CSmartPtr<nm_framework::ISocket> sock_ptr_t;

}
#endif /* __SN_SOCKET_H__ */
