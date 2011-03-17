/*
 * sn_listener.h
 *
 *  Created on: Feb 24, 2011
 *      Author: rock
 */

#ifndef SN_LISTENER_H_
#define SN_LISTENER_H_

#include <map>

#include <utils/smart_lock.h>

#include "sn_socket.h"
#include "../framework/sn_listener.h"
#include "../engine/sn_engine.h"
#include "../framework/sn_endpoint.h"

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
	nm_engine::engine_ptr_t m_pengine;
	///
	template<typename _Tp>
	struct SLessForAddr: public std::binary_function<_Tp, _Tp, bool>
	{
		bool operator()(const _Tp& __x, const _Tp& __y) const
		{
			return __x.get_ip_hbo() < __y.get_ip_hbo() ? true
					: __x.get_port_hbo() < __y.get_port_hbo();
		}
	};
	typedef std::map<nm_network::CIpv4Addr, sn_framework::endpoint_ptr_t, SLessForAddr<nm_network::CIpv4Addr> > tcp_endpoint_map_t;
	tcp_endpoint_map_t m_maptcpendpoint;
	typedef std::set<nm_framework::endpoint_ptr_t> tcp_endpoint_set_t;
	CSpinLock m_lktcpendpoints;

};
typedef nm_utils::CSmartPtr<nm_smartnet::CTcpListener> tcp_listener_ptr_t;

}

#endif /* SN_LISTENER_H_ */
