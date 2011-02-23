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

#include "../framework/sn_engine.h"

namespace nm_smartnet
{

using namespace nm_framework;

class CSmartNet
{
public:
	CSmartNet();
	~CSmartNet();

public:
	int32_t start(u_int32_t ui32IoThreadCnt, int32_t i32IoEvtNotifier, int32_t i32MsTimeout);
	int32_t stop();

	net_engine_ptr_t& get_net_engine();
private:
	net_engine_ptr_t m_pNetEngine;
};

}

#endif /* SMART_NET_H_ */
