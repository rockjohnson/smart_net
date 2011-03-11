/*
 * sn_io_engine.cpp
 *
 *  Created on: Mar 2, 2011
 *      Author: rock
 */

#include "sn_engine.h"
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

	input_handle_task_ptr_t pitask = NULL;
	nm_thread::thread_ptr_t pthread = NULL;
	int32_t i32ret = CMNERR_SUC;
	int32_t i32idx = 0;
	while (ui32inputthreadcnt-- > 0)
	{
		pitask = SYS_NOTRW_NEW(CInputHandleTask);
		if (NULL == pitask)
		{
			i32ret = CMNERR_COMMON_ERR;
			break;
		}
		i32ret = pitask->init(i32ioevtnotifier, i32MStimeout, i32idx++);
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

		i32ret = pthread->assign_task(pitask);
		if (CMNERR_SUC != i32ret)
		{
			break;
		}

		i32ret = pthread->start();
		if (CMNERR_SUC != i32ret)
		{
			break;
		}
		m_vecinputtasks.push_back(pitask); ///not judge the return?
		m_vecthreads.push_back(pthread);
	}

	if (CMNERR_SUC != i32ret)
	{
		stop();

		return CMNERR_COMMON_ERR;
	}

	pthread = NULL;
	i32idx = 0;
	output_handle_task_ptr_t potask = NULL;
	while (ui32outputthreadcnt-- > 0)
	{
		potask = SYS_NOTRW_NEW(COutputHandleTask);
		if (NULL == potask)
		{
			i32ret = CMNERR_COMMON_ERR;
			break;
		}
		i32ret = potask->init(i32ioevtnotifier, i32MStimeout, i32idx++);
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

		i32ret = pthread->assign_task(potask);
		if (CMNERR_SUC != i32ret)
		{
			break;
		}

		i32ret = pthread->start();
		if (CMNERR_SUC != i32ret)
		{
			break;
		}
		m_vecoutputtasks.push_back(potask); ///not judge the return?
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
	for (input_task_vec_t::iterator iter = m_vecinputtasks.begin(); iter != m_vecinputtasks.end(); iter++)
	{
		(*iter)->destroy();
	}
	for (input_task_vec_t::iterator iter = m_vecoutputtasks.begin(); iter != m_vecoutputtasks.end(); iter++)
	{
		(*iter)->destroy();
	}
	m_pmisctasks = NULL;
	m_vecinputtasks.clear();
	m_vecoutputtasks.clear();

	return CMNERR_SUC;
}

/**
 *
 * */
int32_t CEngine::add_io_obj(const io_obj_ptr_t &pioobj)
{
	int32_t i32ret = CMNERR_SUC;

	if (pioobj->get_misc_evts() != 0)
	{
		SYS_ASSERT(NULL != m_pmisctasks);
		if (NULL == m_pmisctasks)
		{
			return CMNERR_COMMON_ERR;
		}

		i32ret = m_pmisctasks->add_io_obj(pioobj);
		IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC > i32ret, i32ret);
	}

	if (pioobj->get_output_evts() != 0)
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

		pioobj->set_output_task_id(iMinIdx);
		i32ret = m_vecoutputtasks[iMinIdx]->add_io_obj(pioobj);
		IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC > i32ret, i32ret);
	}

	if (pioobj->get_input_evts() != 0)
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

		pioobj->set_input_task_id(iMinIdx);
		i32ret = m_vecinputtasks[iMinIdx]->add_io_obj(pioobj);
		IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC > i32ret, i32ret);
	}

	return CMNERR_SUC;
}

/**
 *
 * */
int32_t CEngine::del_io_obj(const io_obj_ptr_t &pioobj)
{
	int32_t i32ret = CMNERR_SUC;

	if (pioobj->get_misc_evts() != 0)
	{
		SYS_ASSERT(NULL != m_pmisctasks);
		if (NULL == m_pmisctasks)
		{
			return CMNERR_COMMON_ERR;
		}

		i32ret = m_pmisctasks->del_io_obj(pioobj);
		IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC > i32ret, i32ret);
	}

	if (pioobj->get_input_evts() != 0)
	{
		i32ret = m_vecinputtasks[pioobj->get_input_task_id()]->del_io_obj(pioobj);
		IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC > i32ret, i32ret);
	}

	if (pioobj->get_output_evts() != 0)
	{
		i32ret = m_vecoutputtasks[pioobj->get_output_task_id()]->del_io_obj(pioobj);
		IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC > i32ret, i32ret);
	}

	return CMNERR_SUC;
}

/**
 *
 * */
int32_t CEngine::add_timer(const timer_ptr_t &ptimer)
{
	SYS_ASSERT(NULL != m_pmisctasks);
	if (NULL == m_pmisctasks)
	{
		return CMNERR_COMMON_ERR;
	}

	return m_pmisctasks->add_timer(ptimer);
}

/**
 *
 * */
int32_t CEngine::del_timer(const timer_ptr_t &ptimer)
{
	SYS_ASSERT(NULL != m_pmisctasks);
	if (NULL == m_pmisctasks)
	{
		return CMNERR_COMMON_ERR;
	}

	return m_pmisctasks->del_timer(ptimer);
}



}
