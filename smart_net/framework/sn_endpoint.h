/*
 * sn_endpoint.h
 *
 *  Created on: Mar 7, 2011
 *      Author: rock
 */

#ifndef __SN_ENDPOINT_H__
#define __SN_ENDPOINT_H__

#include <memory/mem.h>

#include "../network/sn_net_addr.h"
#include "../engine/sn_io_obj.h"

namespace nm_framework
{

/**
 * endpoint interface
 * */
class IEndpoint : public nm_engine::IIoObj
{
public:
	IEndpoint();
	virtual ~IEndpoint();

public:
	//virtual int32_t open(nm_network::net_addr_ptr_t &pFirstAddr, nm_network::net_addr_ptr_t &pSecondAddr) = 0; ///open and join net engine.
	virtual bool is_openned() = 0;
	virtual int32_t close() = 0; ///close and delete from net engine.
	virtual int32_t send_data(nm_memory::mem_ptr_t &pData) = 0; ///thread-safe send func.
	virtual int32_t get_type() = 0;
	virtual int32_t get_proto_id() = 0;

protected:
	virtual void on_opened(nm_network::CIpv4Addr &peeraddr) = 0;
	virtual void on_recv_data(nm_memory::mem_ptr_t &pdata, nm_network::CIpv4Addr &peeraddr) = 0;
	virtual void on_error(int32_t i32errcode) = 0;
};
typedef nm_utils::CSmartPtr<nm_framework::IEndpoint> endpoint_ptr_t;

}

#endif /* __SN_ENDPOINT_H__ */
