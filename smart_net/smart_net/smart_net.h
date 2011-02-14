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

#include "sn_engine.h"

namespace nm_smartnet
{

class CSmartNet
{
public:
	CSmartNet();
	~CSmartNet();

public:
	int32_t start(u_int32_t ui32IoThreadCnt);
	int32_t stop();

private:
	nm_framework::net_engine_ptr_t m_pNetEngine;
};

}

#endif /* SMART_NET_H_ */
