/*
 * shared_obj.cpp
 *
 *  Created on: Feb 10, 2011
 *      Author: rock
 */

#include "shared_obj.h"

namespace nm_framework
{

using namespace nm_protocol;

CEngineMgr::CEngineMgr()
:m_vec_proto_wrappers(EP_ALL)
{
	// TODO Auto-generated constructor stub
}

CEngineMgr::~CEngineMgr()
{
	// TODO Auto-generated destructor stub
}

int32_t CEngineMgr::start(u_int32_t ui32InputThreadCnt, u_int32_t ui32OutputThreadCnt, int32_t i32IoEvtNotifier, int32_t i32MsTimeout)
{

}

int32_t CEngineMgr::stop()
{
	///stop all io thread
	for (thread_vec_t::iterator iter = m_vecInputThreads.begin(); iter
			!= m_vecInputThreads.end(); iter++)
	{
		(*iter)->stop_wait();
	}

	for (thread_vec_t::iterator iter = m_vecInputThreads.begin(); iter
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
int32_t CEngineMgr::add_endpoint(const endpoint_ptr_t &pendpoint)
{
	///
	IF_TRUE_THEN_RETURN_CODE(NULL == pendpoint, CMNERR_COMMON_ERR);
	///
	return m_vec_proto_wrappers[pendpoint->get_proto_id()]->add_endpoint(pendpoint);
}

int32_t CEngineMgr::del_endpoint(const endpoint_ptr_t &pendpoint)
{
	///
	IF_TRUE_THEN_RETURN_CODE(NULL == pendpoint, CMNERR_COMMON_ERR);
	///
	return m_vec_proto_wrappers[pendpoint->get_proto_id()]->del_endpoint(pendpoint);
}

}
