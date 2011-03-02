/*
 * sn_base.h
 *
 *  Created on: Mar 2, 2011
 *      Author: rock
 */

#ifndef __SN_BASE_H__
#define __SN_BASE_H__

#include <memory/mem.h>
#include "../network/sn_net_addr.h"
#include "sn_io_obj.h"

namespace nm_framework
{

using namespace nm_utils;
using namespace nm_network;

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
	virtual int32_t close(); ///close and delete from net engine.

	virtual int32_t send_data(mem_ptr_t &pData) = 0; ///thread-safe send func.
	virtual int32_t close() = 0; ///close this endpoint, close the connection, and will not send or receive data.
	virtual int32_t get_type() = 0;
	virtual net_addr_ptr_t& get_first_addr() = 0;
	virtual net_addr_ptr_t& get_second_addr() = 0;

protected:
	virtual void on_connected(INetAddr &remoteAddr) = 0;
	virtual void on_recved_data(mem_ptr_t &pData, INetAddr &srcAddr) = 0;
	virtual void on_occurred_err(int32_t iErrCode) = 0;
};
typedef nm_utils::CSmartPtr<nm_framework::IEndpoint> endpoint_ptr_t;


/**
 * listener interface
 * */
class IListener : public nm_framework::IIoObj
{
public:
	IListener(){}
	virtual ~IListener(){}

public:
	virtual int32_t add_endpoint(endpoint_ptr_t &pEP) = 0;
};
typedef nm_utils::CSmartPtr<nm_framework::IListener> listener_ptr_t;


/**
 * connecter interface
 * */
class IConnecter
{
public:
	IConnecter();
	virtual ~IConnecter();
};


/**
 *
 * */
enum EProtocols
{
	EP_TCP = 0,
	EP_RUP,
	EP_RMP,
	EP_ALL
};

class IProtoWrapper : public nm_base::ICommonBase
{
public:
	IProtoWrapper(){}
	virtual ~IProtoWrapper(){}
};
typedef nm_utils::CSmartPtr<nm_framework::IProtoWrapper> proto_wrapper_ptr_t;

}

#endif /* __SN_BASE_H__ */
