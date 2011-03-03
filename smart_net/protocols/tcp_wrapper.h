/*
 * tcp_wrapper.h
 *
 *  Created on: Feb 24, 2011
 *      Author: rock
 */

#ifndef TCP_WRAPPER_H_
#define TCP_WRAPPER_H_

#include "../framework/sn_interface.h"

namespace nm_protocol
{

using namespace nm_framework;

class CTcpWrapper : public IProtoWrapper
{
public:
	CTcpWrapper();
	~CTcpWrapper();

public:
	int32_t add_endpoint(const endpoint_ptr_t &pEP);
	int32_t del_endpoint(const endpoint_ptr_t &pEP);

private:
	///tcp listeners
	template<typename _Tp>
	struct lessForAddr: public binary_function<_Tp, _Tp, bool>
	{
		bool operator()(const _Tp& __x, const _Tp& __y) const
		{
			return __x->get_ip_hbo() < __y->get_ip_hbo() ? true
					: __x->get_port_hbo() < __y->get_port_hbo();
		}
	};
	typedef std::map<net_addr_ptr_t, listener_ptr_t, lessForAddr<net_addr_ptr_t> > tcp_listener_map_t;
	tcp_listener_map_t m_mapTcpListeners;
	CMutexLock m_lkTcpListeners;
};








}

#endif /* TCP_WRAPPER_H_ */
