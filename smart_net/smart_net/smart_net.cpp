/*
 * smart_net.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 */

#if 0
#include "smart_net.h"

namespace nm_smartnet
{

using namespace nm_framework;

CSmartNet::CSmartNet()
{
	// TODO Auto-generated constructor stub
}

CSmartNet::~CSmartNet()
{
	// TODO Auto-generated destructor stub
}

int32_t CSmartNet::start(u_int32_t ui32IoThreadCnt, int32_t i32IoEvtNotifier, int32_t i32MsTimeout)
{
	m_pSmartNetMgr = new CSmartNetMgr;
	IF_TRUE_THEN_RETURN_CODE(NULL == m_pSmartNetMgr, CMNERR_COMMON_ERR);

	return m_pSmartNetMgr->start(ui32IoThreadCnt, i32IoEvtNotifier, i32MsTimeout);
}

int32_t CSmartNet::stop()
{
	int32_t i32Ret = m_pSmartNetMgr->stop();
	m_pSmartNetMgr = NULL;

	return i32Ret;
}

smart_net_mgr_ptr_t& CSmartNet::get_mgr()
{
	return m_pSmartNetMgr;
}

}

#endif
