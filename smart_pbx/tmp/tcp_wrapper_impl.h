/*
 * tcp_wrapper.h
 *
 *  Created on: Feb 24, 2011
 *      Author: rock
 */

#ifndef __TCP_WRAPPER_H__
#define __TCP_WRAPPER_H__

#include <map>

#include <utils/smart_lock.h>

#include "../network/sn_net_addr.h"
#include "../framework/sn_proto_wrapper.h"
#include "../engine/sn_engine.h"
#include "../network/sn_listener_impl.h"

namespace nm_protocol
{

class CTcpWrapper : public nm_framework::IProtoWrapper
{
public:
	CTcpWrapper();
	virtual ~CTcpWrapper();

public:
	virtual int32_t init(const nm_engine::engine_ptr_t &pengine);
	virtual int32_t destroy();
	///
	virtual int32_t add_endpoint(const nm_framework::endpoint_ptr_t &pendpoint);
	virtual int32_t del_endpoint(const nm_framework::endpoint_ptr_t &pendpoint);

private:
	///tcp listeners
	template<typename _Tp>
	struct SLessForAddr: public std::binary_function<_Tp, _Tp, bool>
	{
		bool operator()(const _Tp& __x, const _Tp& __y) const
		{
			return __x.get_ip_nbo() < __y.get_ip_nbo() ? true
					: __x.get_port_hbo() < __y.get_port_hbo();
		}
	};
	typedef std::map<nm_network::CIpv4Addr, nm_framework::listener_ptr_t, SLessForAddr<nm_network::CIpv4Addr> > tcp_listener_map_t;
	tcp_listener_map_t m_map_tcp_listeners;
	nm_utils::CMutexLock m_lk_tcp_listeners;
	nm_engine::engine_ptr_t m_pengine;
};

}

#endif /* __TCP_WRAPPER_H__ */
