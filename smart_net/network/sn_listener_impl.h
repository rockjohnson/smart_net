/*
 * sn_listener.h
 *
 *  Created on: Feb 24, 2011
 *      Author: rock
 */

#ifndef SN_LISTENER_H_
#define SN_LISTENER_H_

#include <utils/smart_lock.h>

#include "sn_socket.h"
#include "../framework/sn_listener.h"

namespace nm_network
{

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
	virtual int32_t open(const CIpv4Addr &listenaddr,
			int32_t i32backlog,
			const nm_engine::engine_ptr_t &pioengine);
	virtual int32_t close();
	///
	virtual int32_t add_endpoint(const endpoint_ptr_t &pEP);
	virtual int32_t del_endpoint(const endpoint_ptr_t &pEP);
	///
	virtual void handle_input_evt();
	virtual void handle_output_evt();
	virtual void handle_error_evt();
	int32_t get_fd();

private:
	nm_network::CTcpSock m_tcpsock;
	nm_network::CIpv4Addr m_listenaddr;
	int32_t m_i32backlog;
	nm_framework::io_engine_ptr_t m_pioengine;
	///
	typedef std::set<sn_engine::endpoint_ptr_t> tcp_endpoint_set_t;
	tcp_endpoint_set_t m_setTcpEndpoints;
	CSpinLock m_lktcpendpoints;
};
typedef nm_utils::CSmartPtr<nm_smartnet::CTcpListener> tcp_listener_ptr_t;

}

#endif /* SN_LISTENER_H_ */
