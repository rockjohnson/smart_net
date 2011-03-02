/*
 * sn_listener.h
 *
 *  Created on: Feb 24, 2011
 *      Author: rock
 */

#ifndef SN_LISTENER_H_
#define SN_LISTENER_H_

#include <utils/smart_lock.h>
#include "../smart_net/sn_endpoint.h"

namespace nm_network
{

using namespace nm_utils;
using namespace nm_smartnet;
using namespace nm_framework;

/**
 * tcp listener
 * */
class CTcpListener : public nm_framework::IListener
{
public:
	CTcpListener();
	virtual ~CTcpListener();

public:
	///
	int32_t open(net_addr_ptr_t &pBindAddr, int32_t i32Backlog, io_engine_ptr_t &pNetEngine);
	int32_t close();
	///
	int32_t add_endpoint(const endpoint_ptr_t &pEP);
	int32_t del_endpoint(const endpoint_ptr_t &pEP);
	///
	void handle_input_evt();
	void handle_output_evt();
	void handle_error_evt();
	int32_t get_fd();

private:
	CTcpSock m_tcpSock;
	net_addr_ptr_t m_pBindAddr;
	io_engine_ptr_t m_pNetEngine;
	///
	typedef std::set<tcp_ib_endpoint_ptr_t> tcp_endpoint_set_t;
	tcp_endpoint_set_t m_setTcpEndpoints;
	CSpinLock m_lkTcpEndpoints;
};
typedef nm_utils::CSmartPtr<nm_smartnet::CTcpListener> tcp_listener_ptr_t;

}

#endif /* SN_LISTENER_H_ */
