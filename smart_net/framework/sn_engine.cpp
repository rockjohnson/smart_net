/*
 * shared_obj.cpp
 *
 *  Created on: Feb 10, 2011
 *      Author: rock
 */

#include "sn_engine.h"

namespace nm_framework
{

CNetEngine::CNetEngine()
{
	// TODO Auto-generated constructor stub
}

CNetEngine::~CNetEngine()
{
	// TODO Auto-generated destructor stub
}

int32_t CNetEngine::start(u_int32_t ui32IoThreadCnt, int32_t i32IoEvtNotifier)
{
	if (0 == ui32IoThreadCnt)
	{
		return CMNERR_COMMON_ERR;
	}

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
		i32Ret = pIoTask->init(i32IoEvtNotifier);
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

int32_t CNetEngine::stop()
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

}
