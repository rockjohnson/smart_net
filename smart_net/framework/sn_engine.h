/*
 * shared_obj.h
 *
 *  Created on: Feb 10, 2011
 *      Author: rock
 */

#ifndef __SHARED_OBJ_H__
#define __SHARED_OBJ_H__

#include "../network/sn_net_addr.h"
#include "sn_io_task.h"
#include "sn_base.h"
#include "sn_io_engine.h"
#include <vector>

namespace nm_framework
{

using namespace nm_network;

/**
 * the smart net manager.
 * */
class CNetEngine: public nm_base::ICommonBase
{
public:
	CNetEngine();
	virtual ~CNetEngine();

public:
	int32_t start(u_int32_t ui32IoThreadCnt, int32_t i32IoEvtNotifier, int32_t i32MsTimeout);
	int32_t stop();

	int32_t add_io_obj(const io_obj_ptr_t &pIoObj);
	int32_t del_io_obj(const io_obj_ptr_t &pIoObj);

	int32_t add_endpoint(const endpoint_ptr_t &pIoObj);
	int32_t del_endpoint(const endpoint_ptr_t &pIoObj);

	//			int32_t add_outbound_endpoint(net_addr_ptr_t &pBindAddr, net_addr_ptr_t &pPeerAddr, const io_obj_ptr_t &pIoObj);
	//			int32_t del_outbound_endpoint(const io_obj_ptr_t &pIoObj);

private:
	///io engines
	io_engine_ptr_t m_pIoEngine;

	///network protocol engines
	typedef std::vector<proto_wrapper_ptr_t> proto_wrapper_vec_t;
	proto_wrapper_vec_t m_vecProtoWrappers;


//	///tcp listeners
//	template<typename _Tp>
//	struct lessForAddr: public binary_function<_Tp, _Tp, bool>
//	{
//		bool operator()(const _Tp& __x, const _Tp& __y) const
//		{
//			return __x->get_ip_hbo() < __y->get_ip_hbo() ? true : __x->get_port_hbo() < __y->get_port_hbo();
//		}
//	};
//	typedef std::map<net_addr_ptr_t, listener_ptr_t, lessForAddr<net_addr_ptr_t> > tcp_listener_map_t;
//	tcp_listener_map_t m_mapTcpListeners;
//	CMutexLock m_lkTcpListeners;

};
typedef nm_utils::CSmartPtr<nm_framework::CNetEngine> io_engine_ptr_t;

}

#endif /* __SHARED_OBJ_H__ */
