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
#include "../smart_net/sn_endpoint.h"

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
	///
	int32_t start(net_addr_ptr_t &pBindAddr, int32_t i32Backlog, net_engine_ptr_t &pNetEngine);
	int32_t stop();
	///
	void handle_input_evt();
	void handle_output_evt();
	void handle_error_evt();
	int32_t get_fd();

private:
	CTcpSock m_tcpSock;
	net_addr_ptr_t m_pBindAddr;
	net_engine_ptr_t m_pNetEngine;
};
typedef nm_utils::CSmartPtr<nm_smartnet::CTcpListener> tcp_listener_ptr_t;


}

#endif /* NET_LISTENER_H_ */
