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
#include "sn_io_obj.h"

namespace nm_framework
{

#if (0)
/**
 * endpoint interface
 * */
class IEndpoint : public nm_framework::IIoObj
{
public:
	IEndpoint();
	virtual ~IEndpoint();

public:
	virtual bool is_openned() = 0;
	virtual int32_t close() = 0; ///close and delete from net engine.
	virtual int32_t send_data(nm_mem::mem_ptr_t &pData) = 0; ///thread-safe send func.
	virtual int32_t get_type() = 0;
	virtual int32_t get_proto_id() = 0;
	virtual nm_network::ipv4_addr_ptr_t& get_peer_addr() const = 0;
	virtual nm_network::ipv4_addr_ptr_t& get_local_addr() const = 0;
	virtual int32_t get_input_task_index() const;
	virtual int32_t get_output_task_index() const;

protected:
	virtual void on_opened(nm_network::CIpv4Addr &peeraddr) = 0;
	virtual void on_recv_data(nm_mem::mem_ptr_t &pdata, nm_network::CIpv4Addr &peeraddr) = 0;
	virtual void on_error(int32_t i32errcode) = 0;
	virtual void on_closed() = 0;
};
typedef nm_utils::CSmartPtr<nm_framework::IEndpoint> endpoint_ptr_t;

#endif
}



#endif /* __SN_ENDPOINT_H__ */
