/*
 * shared_obj.cpp
 *
 *  Created on: Feb 10, 2011
 *      Author: rock
 */

#include "sn_engine.h"

namespace nm_framework
{
	struct SParas
	{
		u_int32_t ui32A;
		u_int32_t ui32B;
		int32_t i32C;
		int32_t i32D;
	};

	CSNEngine::CSNEngine()
	:m_sm(this)
	{
		m_sm.reg_evt_state(EES_STOPPED, EEE_START, EES_STARTED, &CSNEngine::starting);
		m_sm.reg_evt_state(EES_STARTED, EEE_STOP, EES_STOPPED, &CSNEngine::stopping);
	}

	CSNEngine::~CSNEngine()
	{
		// TODO Auto-generated destructor stub
	}

	int32_t CSNEngine::start(u_int32_t ui32InputThreadCnt, u_int32_t ui32OutputThreadCnt, int32_t i32IoEvtNotifier, int32_t i32MsTimeout)
	{
		SParas sp;
		sp.ui32A = ui32InputThreadCnt;
		sp.ui32B = ui32OutputThreadCnt;
		sp.i32C = i32IoEvtNotifier;
		sp.i32D = i32MsTimeout;
		return m_sm.post_event(EEE_START, &sp);
	}

	/**
	 *
	 * if this func return err, you should call stop func :)...
	 * */
	int32_t CSNEngine::starting(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, pvoid_t pVoid)
	{
		using namespace nm_thread;

		///
		SParas &sp = *(static_cast<SParas*> (pVoid));
		u_int32_t ui32InputThreadCnt = sp.ui32A;
		u_int32_t ui32OutputThreadCnt = sp.ui32B;
		int32_t i32IoEvtNotifier = sp.i32C;
		int32_t i32MsTimeout = sp.i32D;

		///checking
		IF_TRUE_THEN_RETURN_CODE((0 == ui32InputThreadCnt || 0 == ui32OutputThreadCnt || 0 == i32IoEvtNotifier), RET_ERR);
		///start thread
		///output task
		int32_t i32Ret = 0;
		for (int32_t i = 0; i < ui32OutputThreadCnt; i++)
		{
			output_handle_task_ptr_t pOutputTask = SYS_NOTRW_NEW(COutputHandleTask);
			thread_ptr_t pThread = SYS_NOTRW_NEW(CThread);
			pThread->assign_task(pOutputTask);
			i32Ret = pThread->start();
			IF_TRUE_THEN_RETURN_CODE((i32Ret < 0), RET_ERR);
			m_vecOutputTasks.push_back(pOutputTask);
			pOutputTask->set_indx(m_vecOutputTasks.size() - 1);
			m_vecThreads.push_back(pThread);
		}
		///input task
		for (int32_t i = 0; i < ui32InputThreadCnt; i++)
		{
			input_handle_task_ptr_t pInputTask = SYS_NOTRW_NEW(CInputHandleTask);
			thread_ptr_t pThread = SYS_NOTRW_NEW(CThread);
			pThread->assign_task(pInputTask);
			i32Ret = pThread->start();
			IF_TRUE_THEN_RETURN_CODE((i32Ret < 0), RET_ERR);
			m_vecInputTasks.push_back(pInputTask);
			pInputTask->set_indx(m_vecInputTasks.size() - 1);
			m_vecThreads.push_back(pThread);
		}

		misc_task_ptr_t m_pMiscTask = SYS_NOTRW_NEW(CMiscTask);
		thread_ptr_t pThread = SYS_NOTRW_NEW(CThread);
		pThread->assign_task(m_pMiscTask);
		i32Ret = pThread->start();
		IF_TRUE_THEN_RETURN_CODE((i32Ret < 0), RET_ERR);
		m_vecThreads.push_back(pThread);

		return RET_SUC;
	}

	int32_t CSNEngine::stopping(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, pvoid_t pVoid)
	{
		///stop all io thread
		for (thread_vec_t::iterator iter = m_vecThreads.begin(); iter != m_vecThreads.end(); ++iter)
		{
			(*iter)->stop_wait();
		}

		for (thread_vec_t::iterator iter = m_vecThreads.begin(); iter != m_vecThreads.end(); iter++)
		{
			(*iter)->reset_task();
		}

		m_vecThreads.clear();
		m_vecInputTasks.clear();
		m_vecOutputTasks.clear();
		m_pMiscTask = NULL;

		return CMNERR_SUC;
	}

	int32_t CSNEngine::stop()
	{
		return m_sm.post_event(EEE_STOP, NULL);
	}

	/**
	 * thread safe
	 * */
	int32_t CSNEngine::add_endpoint(const endpoint_ptr_t &pEndpoint)
	{
		using namespace nm_framework;
		///
		IF_TRUE_THEN_RETURN_CODE(NULL == pEndpoint, CMNERR_COMMON_ERR);
		///cool!!
		IF_TRUE_THEN_RETURN_CODE(m_sm.begin_lock_state(EES_STARTED), CMNERR_COMMON_ERR);

		///assign input task, thread safe?
		int32_t i32MinCnt = 0;
		int32_t i32Tmp = 0;
		input_handle_task_ptr_t pInputTask;
		for (input_task_vec_t::iterator iter = m_vecInputTasks.begin(); iter != m_vecInputTasks.end(); ++iter)
		{
			i32Tmp = (*iter)->get_ioobj_cnt();
			if (0 == i32Tmp)
			{
				pInputTask = (*iter);
				break;
			}
			else if (i32MinCnt > i32Tmp)
			{
				i32MinCnt = i32Tmp;
				pInputTask = (*iter);
			}
		}
		pInputTask->add_io_obj(pEndpoint);

		///output task
		output_handle_task_ptr_t pOutputTask;
		for (output_task_vec_t::iterator iter = m_vecOutputTasks.begin(); iter != m_vecOutputTasks.end(); ++iter)
		{
			i32Tmp = (*iter)->get_ioobj_cnt();
			if (0 == i32Tmp)
			{
				pOutputTask = (*iter);
				break;
			}
			else if (i32MinCnt > i32Tmp)
			{
				i32MinCnt = i32Tmp;
				pOutputTask = (*iter);
			}
		}
		pOutputTask->add_io_obj(pEndpoint);

		m_sm.end_lock_state();

		return CMNERR_SUC;
	}

	int32_t CSNEngine::del_endpoint(const endpoint_ptr_t &pEndpoint)
	{
		///
		IF_TRUE_THEN_RETURN_CODE(NULL == pEndpoint, CMNERR_COMMON_ERR);
		///cool!!
		IF_TRUE_THEN_RETURN_CODE(m_sm.begin_lock_state(EES_STARTED), CMNERR_COMMON_ERR);

		///assign input task, thread safe?
		m_vecInputTasks[pEndpoint->get_input_task_index()]->del_io_obj(pEndpoint);
		m_vecOutputTasks[pEndpoint->get_output_task_index()]->del_io_obj(pEndpoint);

		m_sm.end_lock_state();
	}
}
