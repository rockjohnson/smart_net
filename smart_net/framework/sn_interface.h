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
