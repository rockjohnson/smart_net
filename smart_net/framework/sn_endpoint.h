/*
 * sn_endpoint.h
 *
 *  Created on: Mar 7, 2011
 *      Author: rock
 */

#ifndef __SN_ENDPOINT_H__
#define __SN_ENDPOINT_H__

#include "sn_io_obj.h"

namespace nm_framework
{

/**
 * endpoint interface
 * */
class IEndpoint : public nm_framework::IIoObj
{
public:
	IEndpoint();
	virtual ~IEndpoint();

public:
	virtual int32_t open(net_addr_ptr_t &pFirstAddr, net_addr_ptr_t &pSecondAddr) = 0; ///open and join net engine.
	virtual int32_t close() = 0; ///close and delete from net engine.

	virtual int32_t send_data(mem_ptr_t &pData) = 0; ///thread-safe send func.
	virtual int32_t get_type() = 0;
	virtual int32_t get_proto() = 0;

protected:
	virtual void on_opened(INetAddr &remoteAddr) = 0;
	virtual void on_closed() = 0;
	virtual void on_recved_data(mem_ptr_t &pData, INetAddr &srcAddr) = 0;
	virtual void on_occurred_err(int32_t iErrCode) = 0;
};
typedef nm_utils::CSmartPtr<nm_framework::IEndpoint> endpoint_ptr_t;

}

#endif /* __SN_ENDPOINT_H__ */
