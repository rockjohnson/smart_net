/*
 * net_listener.h
 *
 *  Created on: Feb 12, 2011
 *      Author: rock
 */

#ifndef NET_LISTENER_H_
#define NET_LISTENER_H_

#include "socket.h"
#include "../framework/io_obj.h"

namespace nm_network
{

/**
 * base net listner
 * */
class INetListener : public nm_framework::IIoObj
{
public:
	INetListener();
	virtual ~INetListener();
};

class CTcpListener : public INetListener
{
public:
	CTcpListener();
	virtual ~CTcpListener();

public:
	int32_t init(INetAddr &bindAddr, int32_t i32Backlog);
	int32_t destroy();
	void handle_input_evt();
	void handle_output_evt();
	void handle_error_evt();

	int32_t get_fd();

private:
	CTcpSock m_tcpSock;
};
typedef nm_utils::CSmartPtr<nm_network::CTcpListener> tcp_listener_ptr_t;


}

#endif /* NET_LISTENER_H_ */
