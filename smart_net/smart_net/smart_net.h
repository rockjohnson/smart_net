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

#include "../network/sn_endpoint.h"

namespace nm_smartnet
{

using namespace nm_framework;
using namespace nm_network;

class CSmartNet : public nm_base::ICommonBase
{
public:
	CSmartNet();
	~CSmartNet();

public:
	///
	int32_t start(u_int32_t ui32IoThreadCnt, int32_t i32IoEvtNotifier, int32_t i32MsTimeout);
	int32_t stop();
	///
	smart_net_mgr_ptr_t& get_mgr();
private:
	smart_net_mgr_ptr_t m_pSmartNetMgr;
};
typedef nm_utils::CSmartPtr<nm_smartnet::CSmartNet> smart_net_ptr_t;

}

#endif /* SMART_NET_H_ */
