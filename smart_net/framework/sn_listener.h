/*
 * sn_listener.h
 *
 *  Created on: Mar 7, 2011
 *      Author: rock
 */

#ifndef __SN_LISTENER_H__
#define __SN_LISTENER_H__

#include "sn_io_obj.h"

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
	virtual int32_t add_endpoint(endpoint_ptr_t &pEP) = 0;
};
typedef nm_utils::CSmartPtr<nm_framework::IListener> listener_ptr_t;

}

#endif /* __SN_LISTENER_H__ */
