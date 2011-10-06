/*
 * sn_base.h
 *
 *  Created on: Mar 2, 2011
 *      Author: rock
 */

#ifndef __SN_BASE_H__
#define __SN_BASE_H__

#include "../common/sn_common.h"
#include "sn_endpoint.h"

namespace nm_framework
{

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

public:
	virtual int32_t add_endpoint(const endpoint_ptr_t &pendpoint) = 0;
	virtual int32_t del_endpoint(const endpoint_ptr_t &pendpoint) = 0;
};
typedef nm_utils::CSmartPtr<nm_framework::IProtoWrapper> proto_wrapper_ptr_t;

}

#endif /* __SN_BASE_H__ */
