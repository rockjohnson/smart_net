/*
 * tcp_wrapper.h
 *
 *  Created on: Feb 24, 2011
 *      Author: rock
 */

#ifndef __TCP_WRAPPER_H__
#define __TCP_WRAPPER_H__

#include "../framework/sn_proto_wrapper.h"
#include "../framework/sn_io_engine.h"

namespace nm_protocol
{

class CTcpWrapper : public nm_framework::IProtoWrapper
{
public:
	CTcpWrapper();
	virtual ~CTcpWrapper();

public:
	virtual int32_t init(const nm_framework::io_engine_ptr_t &pioengine);
	virtual int32_t destroy();

public:
	virtual int32_t add_endpoint(const nm_framework::endpoint_ptr_t &pEP);
	virtual int32_t del_endpoint(const nm_framework::endpoint_ptr_t &pEP);

private:
	///tcp listeners
	template<typename _Tp>
	struct SLessForAddr: public binary_function<_Tp, _Tp, bool>
	{
		bool operator()(const _Tp& __x, const _Tp& __y) const
		{
			return __x->get_ip_hbo() < __y->get_ip_hbo() ? true
					: __x->get_port_hbo() < __y->get_port_hbo();
		}
	};
	typedef std::map<net_addr_ptr_t, listener_ptr_t, SLessForAddr<net_addr_ptr_t> > tcp_listener_map_t;
	tcp_listener_map_t m_mapTcpListeners;
	CMutexLock m_lktcplisteners;
	nm_framework::io_engine_ptr_t m_pioengine;
};

}

#endif /* __TCP_WRAPPER_H__ */
