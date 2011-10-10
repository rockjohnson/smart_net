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
#include <utils/event_engine.h>

#include "sn_listener.h"

namespace nm_network
{

class nm_network::CTcpSock;
class nm_smartnet::CTcpInboundEndpoint;
/**
 * tcp listener
 * */
class CTcpListener : public nm_framework::IListener
{
public:
	CTcpListener();
	virtual ~CTcpListener();

	typedef nm_utils::CSmartPtr<nm_smartnet::CTcpInboundEndpoint> tcp_ib_endpoint_ptr_t;
public:
	///
	virtual int32_t open(const CIpv4Addr &listenaddr,
			int32_t i32backlog,
			const nm_engine::engine_ptr_t &pengine);
	virtual int32_t close();
	///
	virtual int32_t add_endpoint(const nm_framework::endpoint_ptr_t &pep);
	virtual int32_t del_endpoint(const nm_framework::endpoint_ptr_t &pep);
			int32_t add_endpoint_ex(const nm_framework::endpoint_ptr_t &pep);
			int32_t add_endpoint_ex(const nm_framework::endpoint_ptr_t &pep);
	///
	virtual void handle_input_evt();
	virtual void handle_output_evt();
	virtual void handle_error_evt();

	virtual sock_handle_t get_handle();
	virtual u_int32_t get_input_evts();
	virtual u_int32_t get_output_evts();
	virtual u_int32_t get_misc_evts();
	virtual void set_input_task_id(int32_t i32id);
	virtual int32_t get_input_task_id();
	virtual void set_output_task_id(int32_t i32id);
	virtual int32_t get_output_task_id();

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
	typedef std::map<nm_network::CIpv4Addr, tcp_ib_endpoint_ptr_t, SLessForAddr<nm_network::CIpv4Addr> > tcp_ib_endpoint_map_t;
	tcp_ib_endpoint_map_t m_maptcpibendpoint;
	typedef std::set<tcp_ib_endpoint_ptr_t> tcp_ib_endpoint_set_t;
	tcp_ib_endpoint_set_t m_settcpibendpoint;
	nm_utils::CSpinLock m_lktcpendpoints;
};
typedef nm_utils::CSmartPtr<nm_network::CTcpListener> tcp_listener_ptr_t;

///
class CTcpListenerEvt : public nm_utils::IEvent
{
public:
	enum
	{
		EADD_EP = 0,
		EDEL_EP
	};

public:
	CTcpListenerEvt(tcp_listener_ptr_t &ptcplistener, nm_framework::endpoint_ptr_t &pendpoint, int32_t i32opcode);
	virtual ~CTcpListenerEvt();

public:
	void handle();

private:
	int32_t m_i32opcode;
	tcp_listener_ptr_t m_ptcplistener;
	nm_framework::endpoint_ptr_t m_pendpoint;
};
typedef nm_utils::CSmartPtr<nm_network::CTcpListenerEvt> tcp_listener_evt_ptr_t;

}

#endif /* SN_LISTENER_H_ */
