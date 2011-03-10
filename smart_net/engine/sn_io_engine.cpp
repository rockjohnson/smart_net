/*
 * sn_io_engine.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: rock
 */

#include "sn_io_engine.h"
#include "../common/sn_common.h"

namespace nm_engine
{

/**
 *
 * */
CEngine::CEngine()
{
	// TODO Auto-generated constructor stub
}

/**
 *
 * */
CEngine::~CEngine()
{
	// TODO Auto-generated destructor stub
}

/**
 *
 * */
int32_t CEngine::start(u_int32_t ui32inputthreadcnt,
		u_int32_t ui32outputthreadcnt, bool bmiscthread, int32_t i32ioevtnotifier,
		int32_t i32MStimeout)
{
	IF_TRUE_THEN_RETURN_CODE(0 == ui32inputthreadcnt
			|| 0 == ui32outputthreadcnt, CMNERR_COMMON_ERR);

	IF_TRUE_THEN_RETURN_CODE(EIEN_NONE >= i32ioevtnotifier
			|| EIEN_ALL <= i32ioevtnotifier, CMNERR_COMMON_ERR);

	//	///create protocol wrapper
	//	m_vecProtoWrappers[EP_TCP] = SYS_NOTRW_NEW(CTcpWrapper);

	io_task_ptr_t piotask = NULL;
	nm_thread::thread_ptr_t pthread = NULL;
	int32_t i32ret = CMNERR_SUC;
	while (ui32inputthreadcnt-- > 0)
	{
		piotask = SYS_NOTRW_NEW(CIoTask);
		if (NULL == piotask)
		{
			i32ret = CMNERR_COMMON_ERR;
			break;
		}
		i32ret = piotask->init(i32ioevtnotifier, i32MStimeout);
		if (CMNERR_SUC != i32ret)
		{
			break;
		}
		//m_setIoTasks.push_back(pIoTask);

		pthread = SYS_NOTRW_NEW(CThread);
		if (NULL == pthread)
		{
			i32ret = CMNERR_COMMON_ERR;
			break;
		}

		i32ret = pthread->init();
		if (CMNERR_SUC != i32ret)
		{
			break;
		}

		i32ret = pthread->assign_task(piotask);
		if (CMNERR_SUC != i32ret)
		{
			break;
		}

		i32ret = pthread->start();
		if (CMNERR_SUC != i32ret)
		{
			break;
		}
		m_vecinputtasks.push_back(piotask); ///not judge the return?
		m_vecthreads.push_back(pthread);
	}

	if (CMNERR_SUC != i32ret)
	{
		stop();

		return CMNERR_COMMON_ERR;
	}

	pthread = NULL;
	piotask = NULL;
	while (ui32outputthreadcnt-- > 0)
	{
		piotask = SYS_NOTRW_NEW(CIoTask);
		if (NULL == piotask)
		{
			i32ret = CMNERR_COMMON_ERR;
			break;
		}
		i32ret = piotask->init(i32ioevtnotifier, i32MStimeout);
		if (CMNERR_SUC != i32ret)
		{
			break;
		}
		//m_setIoTasks.push_back(pIoTask);

		pthread = SYS_NOTRW_NEW(CThread);
		if (NULL == pthread)
		{
			i32ret = CMNERR_COMMON_ERR;
			break;
		}

		i32ret = pthread->init();
		if (CMNERR_SUC != i32ret)
		{
			break;
		}

		i32ret = pthread->assign_task(piotask);
		if (CMNERR_SUC != i32ret)
		{
			break;
		}

		i32ret = pthread->start();
		if (CMNERR_SUC != i32ret)
		{
			break;
		}
		m_vecoutputtasks.push_back(piotask); ///not judge the return?
		m_vecthreads.push_back(pthread);
	}

	if (CMNERR_SUC != i32ret)
	{
		stop();

		return CMNERR_COMMON_ERR;
	}

	if (bmiscthread)
	{
		do
		{
			misc_task_ptr_t pmisctask = SYS_NOTRW_NEW(CMiscTask);
			if (NULL == pmisctask)
			{
				i32ret = CMNERR_COMMON_ERR;
				break;
			}

			i32ret = pmisctask->init(i32ioevtnotifier, i32MStimeout);
			if (CMNERR_SUC != i32ret)
			{
				break;
			}
			//m_setIoTasks.push_back(pIoTask);

			pthread = SYS_NOTRW_NEW(CThread);
			if (NULL == pthread)
			{
				i32ret = CMNERR_COMMON_ERR;
				break;
			}

			i32ret = pthread->init();
			if (CMNERR_SUC != i32ret)
			{
				break;
			}

			i32ret = pthread->assign_task(pmisctask);
			if (CMNERR_SUC != i32ret)
			{
				break;
			}

			i32ret = pthread->start();
			if (CMNERR_SUC != i32ret)
			{
				break;
			}
		} while (false);

		if (CMNERR_SUC != i32ret)
		{
			stop();

			return CMNERR_COMMON_ERR;
		}
	}

	return i32ret;
}

/**
 * stop this engine.
 * */
int32_t CEngine::stop()
{
	///stop all the threads
	for (thread_vec_t::iterator iter = m_vecthreads.begin(); iter != m_vecthreads.end(); iter++)
	{
		(*iter)->stop_wait();
	}
	m_vecthreads.clear();

	///destroy all the tasks
	if (NULL != m_pmisctasks)
	{
		m_pmisctasks->destroy();
	}
	for (io_task_vec_t::iterator iter = m_vecinputtasks.begin(); iter != m_vecinputtasks.end(); iter++)
	{
		(*iter)->destroy();
	}
	for (io_task_vec_t::iterator iter = m_vecoutputtasks.begin(); iter != m_vecoutputtasks.end(); iter++)
	{
		(*iter)->destroy();
	}
	m_pmisctasks = NULL;
	m_vecinputtasks.clear();
	m_vecoutputtasks.clear();

	return CMNERR_SUC;
}

int32_t CEngine::add_io_obj(const io_obj_ptr_t &pIoObj)
{
	int32_t i32Ret = CMNERR_SUC;
	if (pIoObj->get_output_flg() != 0)
	{
		int32_t iMinIdx = 0;
		int32_t iMinCnt = 0;
		for (int32_t i = 0; i < m_vecoutputtasks.size(); i++)
		{
			if (m_vecoutputtasks[i]->get_fd_cnt() < iMinCnt)
			{
				iMinIdx = i;
				iMinCnt = m_vecoutputtasks[i]->get_fd_cnt();
			}
		}

		pIoObj->set_output_task_id(iMinIdx);
		i32Ret = m_vecoutputtasks[iMinIdx]->add_io_obj(pIoObj);
		IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC > i32Ret, i32Ret);
	}

	if (pIoObj->get_input_flg() != 0)
	{
		int32_t iMinIdx = 0;
		int32_t iMinCnt = 0;
		for (int32_t i = 0; i < m_vecinputtasks.size(); i++)
		{
			if (m_vecinputtasks[i]->get_fd_cnt() < iMinCnt)
			{
				iMinIdx = i;
				iMinCnt = m_vecinputtasks[i]->get_fd_cnt();
			}
		}

		pIoObj->set_input_task_id(iMinIdx);
		i32Ret = m_vecinputtasks[iMinIdx]->add_io_obj(pIoObj);
		IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC > i32Ret, i32Ret);
	}

	return CMNERR_SUC;
}

}
