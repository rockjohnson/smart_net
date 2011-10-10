/*
 * sn_listener.h
 *
 *  Created on: Mar 7, 2011
 *      Author: rock
 */

#ifndef __SN_LISTENER_H__
#define __SN_LISTENER_H__

#include "../framework/sn_io_obj.h"
#include "../framework/sn_engine.h"
#include "../network/sn_net_addr.h"
#include "../framework/sn_endpoint.h"

namespace nm_framework
{

/**
 * listener interface
 * */
class IListener : public nm_framework::IIoObj
{
public:
	IListener(){}
	virtual ~IListener(){}

public:
	///
	virtual int32_t open(const nm_network::CIpv4Addr &listenaddr, int32_t i32backlog, const nm_engine::engine_ptr_t &pengine) = 0;
	virtual int32_t close() = 0;
	///
	virtual int32_t add_endpoint(const endpoint_ptr_t &pep) = 0;
	virtual int32_t del_endpoint(const endpoint_ptr_t &pep) = 0;
};
typedef nm_utils::CSmartPtr<nm_framework::IListener> listener_ptr_t;

}

#endif /* __SN_LISTENER_H__ */
