/*
 * shared_obj.cpp
 *
 *  Created on: Feb 10, 2011
 *      Author: rock
 */

#include "sn_engine.h"
#include "../smart_net/sn_endpoint.h"

namespace nm_framework
{

using namespace nm_smartnet;

CNetEngine::CNetEngine()
:m_vecProtoWrappers(EP_ALL)
{
	// TODO Auto-generated constructor stub
}

CNetEngine::~CNetEngine()
{
	// TODO Auto-generated destructor stub
}

int32_t CNetEngine::start(u_int32_t ui32IoThreadCnt, int32_t i32IoEvtNotifier, int32_t i32MsTimeout)
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

/**
 * thread safe
 * */
int32_t CNetEngine::add_endpoint(const endpoint_ptr_t &pEP)
{
	///
	IF_TRUE_THEN_RETURN_CODE(NULL == pEP, CMNERR_COMMON_ERR);
	///
	int32_t i32Ret = CMNERR_SUC;
	switch (pEP->get_type())
	{
	case E_TCP_INBOUND_ENDPOINT:
	{
		i32Ret = m_vecProtoWrappers[EP_TCP]->add_endpoint(pEP);
//		mtx_scopelk_t lk(m_lkTcpListeners);
//		tcp_listener_map_t::iterator iter = m_mapTcpListeners.find(pEP->get_first_addr());
//		if (iter == m_mapTcpListeners.end())
//		{
//			///create tcp listener, and listening.
//			listener_ptr_t pListener = SYS_NOTRW_NEW(CTcpListener);
//			i32Ret = pListener->start(pEP->get_first_addr());
//			if (CMNERR_SUC > i32Ret)
//			{
//				break;
//			}
//			i32Ret = pListener->add_endpoint(pEP);
//			SYS_ASSERT(CMNERR_SUC <= i32Ret);
//			std::pair<tcp_listener_map_t::iterator, bool> ret = m_mapTcpListeners.insert(tcp_listener_map_t::value_type(pEp->get_first_addr(), pEP));
//			SYS_ASSERT(ret.second);
//			i32Ret = add_io_obj(pListener);
//			if (CMNERR_SUC > i32Ret)
//			{
//				break;
//			}
//		}
//		else
//		{
//
//		}

		break;
	}
	case E_TCP_OUTBOUND_ENDPOINT:
	{
		break;
	}
	default:
	{
		break;
	}
	}

	return i32Ret;
}

}
