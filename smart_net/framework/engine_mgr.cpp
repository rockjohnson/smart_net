/*
 * shared_obj.cpp
 *
 *  Created on: Feb 10, 2011
 *      Author: rock
 */

#include "engine_mgr.h"

namespace nm_framework
{
	enum E_ENGINE_STATE
	{
		EES_STOPPED = 0,
		EES_STARTED
	};

	enum E_ENGINE_EVT
	{
		EEE_START = 0,
		EEE_STOP
	};

	struct SParas
	{
		u_int32_t ui32A;
		u_int32_t ui32B;
		int32_t   i32C;
		int32_t   i32D;
	};


	using namespace nm_protocol;

	CEngineMgr::CEngineMgr() :
		m_vec_proto_wrappers(EP_ALL)
	{
		m_sm.reg_evt_state(EES_STOPPED, EEE_START, EES_STARTED, &CEngineMgr::starting);
		m_sm.reg_evt_state(EES_STARTED, EEE_STOP, EES_STOPPED, &CEngineMgr::stopping);
	}

	CEngineMgr::~CEngineMgr()
	{
		// TODO Auto-generated destructor stub
	}

	int32_t CEngineMgr::start(u_int32_t ui32InputThreadCnt,
			u_int32_t ui32OutputThreadCnt, int32_t i32IoEvtNotifier,
			int32_t i32MsTimeout)
	{
		SParas *pTmp = SYS_NOTRW_NEW(SParas);
		pTmp->ui32A = ui32InputThreadCnt;
		pTmp->ui32B = ui32OutputThreadCnt;
		pTmp->i32C  = i32IoEvtNotifier;
		pTmp->i32D  = i32MsTimeout;
		return m_sm.post_event(EEE_START, pTmp);
	}

	/**
	 *
	 * if this func return err, you should call stop func :)...
	 * */
	int32_t CEngineMgr::starting(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, pvoid_t pVoid)
	{
		using namespace nm_engine;
		using namespace nm_thread;

		///
		SParas *pTmp = static_cast<SParas*>(pVoid);
		u_int32_t ui32InputThreadCnt = pTmp->ui32A;
		u_int32_t ui32OutputThreadCnt = pTmp->ui32B;
		int32_t i32IoEvtNotifier = pTmp->i32C;
		int32_t i32MsTimeout = pTmp->i32D;
		delete pTmp;

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
			m_vecThreads.push_back(pThread);
		}

		misc_task_ptr_t m_pMiscTask = SYS_NOTRW_NEW(CInputHandleTask);
		thread_ptr_t pThread = SYS_NOTRW_NEW(CThread);
		pThread->assign_task(m_pMiscTask);
		i32Ret = pThread->start();
		IF_TRUE_THEN_RETURN_CODE((i32Ret < 0), RET_ERR);
		m_vecThreads.push_back(pThread);

		return RET_SUC;
	}

	int32_t CEngineMgr::stopping(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, pvoid_t pVoid)
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
		m_vecInputTasks.clear();
		m_vecOutputTasks.clear();
		m_pMiscTask = NULL;

		return CMNERR_SUC;
	}


	int32_t CEngineMgr::stop()
	{
		return m_sm.post_event(EEE_STOP, NULL);
	}

	/**
	 * thread safe
	 * */
	int32_t CEngineMgr::add_endpoint(const endpoint_ptr_t &pEndpoint)
	{
		///
		IF_TRUE_THEN_RETURN_CODE(NULL == pEndpoint, CMNERR_COMMON_ERR);
		///cool!!
		IF_TRUE_THEN_RETURN_CODE(m_sm.begin_lock_state(EES_STARTED), CMNERR_COMMON_ERR);

		///assign input task, thread safe?
		int32_t i32MinCnt = 0;
		int32_t i32Tmp = 0;
		input_task_ptr_t pInputTask;
		for (input_task_vec_t::iterator iter = m_vecInputTasks.begin(); iter != m_vecInputTasks.end(); ++iter)
		{
			i32Tmp = (*iter)->get_endpoint_cnt();
			if (0 == i32Tmp)
			{
				pInputTask = (*iter);
				break;
			}
			else if (i32MinCnt > iTmp)
			{
				i32MinCnt = iTmp;
				pInputTask = (*iter);
			}
		}
		pInputTask->add_endpoint(pEndpoint);

		///output task
		output_task_ptr_t pOutputTask;
		for (output_task_ptr_t::iterator iter = m_vecOutputTasks.begin(); iter != m_vecOutputTasks.end(); ++iter)
		{
			i32Tmp = (*iter)->get_endpoint_cnt();
			if (0 == i32Tmp)
			{
				pOutputTask = (*iter);
				break;
			}
			else if (i32MinCnt > iTmp)
			{
				i32MinCnt = iTmp;
				pOutputTask = (*iter);
			}
		}
		pOutputTask->add_endpoint(pEndpoint);

		m_sm.end_lock_state();

		return CMNERR_SUC;
	}

	int32_t CEngineMgr::del_endpoint(const endpoint_ptr_t &pendpoint)
	{
		///
		IF_TRUE_THEN_RETURN_CODE(NULL == pendpoint, CMNERR_COMMON_ERR);
		///
		return m_vec_proto_wrappers[pendpoint->get_proto_id()]->del_endpoint(
				pendpoint);
	}

}
