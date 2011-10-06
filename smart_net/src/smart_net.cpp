/*
 * smart_net.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 */


#include "smart_net.h"

namespace nm_smartnet
{

CSmartNet::CSmartNet()
{
	// TODO Auto-generated constructor stub
}

CSmartNet::~CSmartNet()
{
	// TODO Auto-generated destructor stub
}

int32_t CSmartNet::start(u_int32_t ui32inputthreadcnt,
		u_int32_t ui32outputthreadcnt,
		int32_t i32ioevtnotifier, int32_t i32MStimeout)
{
	m_pSmartNetMgr = SYS_NOTRW_NEW(nm_framework::CEngineMgr);
	IF_TRUE_THEN_RETURN_CODE(NULL == m_pSmartNetMgr, CMNERR_COMMON_ERR);

	return m_pSmartNetMgr->start(ui32inputthreadcnt,
			ui32outputthreadcnt, i32ioevtnotifier, i32MStimeout);
}

int32_t CSmartNet::stop()
{
	int32_t i32Ret = m_pSmartNetMgr->stop();
	m_pSmartNetMgr = NULL;
	return i32Ret;
}

int32_t CSmartNet::add_endpoint(nm_framework::endpoint_ptr_t &pep)
{
	return m_pSmartNetMgr->add_endpoint(pep);
}

int32_t CSmartNet::del_endpoint(nm_framework::endpoint_ptr_t &pep)
{
	return m_pSmartNetMgr->del_endpoint(pep);
}

}
