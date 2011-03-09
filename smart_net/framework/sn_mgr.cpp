/*
 * shared_obj.cpp
 *
 *  Created on: Feb 10, 2011
 *      Author: rock
 */

#include "sn_mgr.h"

namespace nm_framework
{

using namespace nm_protocol;

CSmartNetMgr::CSmartNetMgr()
:m_vecProtoWrappers(EP_ALL)
{
	// TODO Auto-generated constructor stub
}

CSmartNetMgr::~CSmartNetMgr()
{
	// TODO Auto-generated destructor stub
}

int32_t CSmartNetMgr::start(u_int32_t ui32InputThreadCnt, u_int32_t ui32OutputThreadCnt, int32_t i32IoEvtNotifier, int32_t i32MsTimeout)
{

}

int32_t CSmartNetMgr::stop()
{
	///stop all io thread
	for (io_thread_vec_t::iterator iter = m_vecInputThreads.begin(); iter
			!= m_vecInputThreads.end(); iter++)
	{
		(*iter)->stop_wait();
	}

	for (io_thread_vec_t::iterator iter = m_vecInputThreads.begin(); iter
			!= m_vecInputThreads.end(); iter++)
	{
		(*iter)->reset_task();
	}

	m_vecInputThreads.clear();

	return CMNERR_SUC;
}

/**
 * thread safe
 * */
int32_t CSmartNetMgr::add_endpoint(const endpoint_ptr_t &pEP)
{
	///
	IF_TRUE_THEN_RETURN_CODE(NULL == pEP, CMNERR_COMMON_ERR);
	///
	return m_vecProtoWrappers[pEP->get_proto()]->add_endpoint(pEP);
}

int32_t CSmartNetMgr::del_endpoint(const endpoint_ptr_t &pEP)
{
	///
	IF_TRUE_THEN_RETURN_CODE(NULL == pEP, CMNERR_COMMON_ERR);
	///
	return m_vecProtoWrappers[pEP->get_proto()]->del_endpoint(pEP);
}

}
