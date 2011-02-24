/*
 * sn_listener.h
 *
 *  Created on: Feb 24, 2011
 *      Author: rock
 */

#ifndef SN_LISTENER_H_
#define SN_LISTENER_H_

namespace nm_network
{

class IListener
{
public:
	IListener();
	virtual ~IListener();
};

/**
 * tcp listener
 * */
class CTcpListener : public nm_framework::IIoObj
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

#endif /* SN_LISTENER_H_ */
