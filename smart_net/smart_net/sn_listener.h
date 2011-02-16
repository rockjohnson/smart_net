/*
 * net_listener.h
 *
 *  Created on: Feb 12, 2011
 *      Author: rock
 */

#ifndef NET_LISTENER_H_
#define NET_LISTENER_H_

#include "../network/sn_socket.h"
#include "../framework/sn_io_obj.h"
#include "sn_endpoint.h"

namespace nm_smartnet
{

/**
 * base net listner
 * */
class IListener : public nm_framework::IIoObj
{
public:
	IListener();
	virtual ~IListener();
};

class CTcpListener : public IListener
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

protected:
	virtual tcp_endpoint_ptr_t create_endpoint() = 0;

private:
	CTcpSock m_tcpSock;
};
typedef nm_utils::CSmartPtr<nm_smartnet::CTcpListener> tcp_listener_ptr_t;


}

#endif /* NET_LISTENER_H_ */
