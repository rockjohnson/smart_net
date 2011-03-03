/*
 * shared_obj.cpp
 *
 *  Created on: Feb 10, 2011
 *      Author: rock
 */

#include "sn_mgr.h"
#include "../network/sn_endpoint.h"
#include "../protocols/tcp_wrapper.h"

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
	if (0 == ui32IoThreadCnt)
	{
		return CMNERR_COMMON_ERR;
	}

	///create protocol wrapper
	m_vecProtoWrappers[EP_TCP] = SYS_NOTRW_NEW(CTcpWrapper);

	io_task_ptr_t pIoTask = NULL;
	thread_ptr_t pThread = NULL;
	int32_t i32Ret = CMNERR_SUC;
	while (ui32IoThreadCnt-- > 0)
	{
		pIoTask = SYS_NOTRW_NEW(CIoTask);
		if (NULL == pIoTask)
		{
			i32Ret = CMNERR_COMMON_ERR;
			break;
		}
		i32Ret = pIoTask->init(i32IoEvtNotifier, i32MsTimeout);
		if (CMNERR_SUC != i32Ret)
		{
			break;
		}
		//m_setIoTasks.push_back(pIoTask);

		pThread = SYS_NOTRW_NEW(CThread);
		if (NULL == pThread)
		{
			i32Ret = CMNERR_COMMON_ERR;
			break;
		}

		i32Ret = pThread->init();
		if (CMNERR_SUC != i32Ret)
		{
			break;
		}

		i32Ret = pThread->assign_task(pIoTask);
		if (CMNERR_SUC != i32Ret)
		{
			break;
		}

		i32Ret = pThread->start();
		if (CMNERR_SUC != i32Ret)
		{
			break;
		}
		m_vecIoThreads.push_back(pThread); ///not judge the return?
	}

	if (CMNERR_SUC != i32Ret)
	{
		stop();
	}

	return i32Ret;
}

int32_t CSmartNetMgr::stop()
{
	///stop all io thread
	for (io_thread_vec_t::iterator iter = m_vecIoThreads.begin(); iter
			!= m_vecIoThreads.end(); iter++)
	{
		(*iter)->stop_wait();
	}

	for (io_thread_vec_t::iterator iter = m_vecIoThreads.begin(); iter
			!= m_vecIoThreads.end(); iter++)
	{
		(*iter)->reset_task();
	}

	m_vecIoThreads.clear();

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

}
