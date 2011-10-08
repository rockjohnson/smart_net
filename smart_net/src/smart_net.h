/*
 * smart_net.h
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 *
 *      smart_net is a powerful network engine.
 */

#ifndef __SMART_NET_H__
#define __SMART_NET_H__

#include "../framework/sn_endpoint.h"
#include "../framework/sn_mgr.h"

namespace nm_smartnet
{

class CSmartNet : public nm_comm_base::ICommonBase
{
public:
	CSmartNet();
	virtual ~CSmartNet();

public:
	///
	int32_t start(u_int32_t ui32inputthreadcnt,
			u_int32_t ui32outputthreadcnt,
			int32_t i32IoEvtNotifier, int32_t i32MsTimeout);
	int32_t stop();
	///
	int32_t add_endpoint(nm_framework::endpoint_ptr_t &pep);
	int32_t del_endpoint(nm_framework::endpoint_ptr_t &pep);

private:
	nm_framework::net_engine_ptr_t m_pSmartNetMgr;
};
typedef nm_utils::CSmartPtr<nm_smartnet::CSmartNet> smart_net_ptr_t;

}

#endif /* SMART_NET_H_ */
