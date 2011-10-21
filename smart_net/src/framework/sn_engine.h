/*
 * shared_obj.h
 *
 *  Created on: Feb 10, 2011
 *      Author: rock
 */

#ifndef __SHARED_OBJ_H__
#define __SHARED_OBJ_H__

#include <deque>

#include <utils/state_machine.h>
#include <thread/thread_ex.h>

#include "../common/sn_common.h"
#include "sn_timer.h"
#include "sn_io_obj.h"
#include "sn_io_task.h"
#include "sn_misc_task.h"


namespace nm_framework
{

	/**
	 * the smart net engine.
	 * */
	class CSNEngine: public nm_cmn_base::ICommonBase
	{
	public:
		CSNEngine();
		virtual ~CSNEngine();

		enum E_ENGINE_STATE
		{
			ES_STOPPED = 0, ES_STARTED
		};

		enum E_ENGINE_EVT
		{
			EE_START = 0, EE_STOP
		};

		DISALLOW_COPY_AND_ASSIGN(CSNEngine);

	public:
		int32_t start(u_int32_t ui32InputThreadCnt, u_int32_t ui32OutputThreadCnt, int32_t i32IoEvtNotifier, int32_t i32MsTimeout);
		int32_t stop();
		///
		int32_t add_endpoint(const io_obj_ptr_t &pEP, int32_t i32IoType); ///not thread safe
		int32_t del_endpoint(const io_obj_ptr_t &pEP, int32_t i32IoType); ///not thread safe
		int32_t add_timer(const timer_obj_ptr_t &pTimer);
		int32_t del_timer(const timer_obj_ptr_t &pTimer);

	private:
		int32_t starting(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);
		int32_t stopping(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid);

	private:
		///state
		nm_utils::CStateMachine<CSNEngine> m_sm;
		///threads
		typedef std::deque<nm_thread::thread_ptr_t> thread_vec_t;
		thread_vec_t m_vecThreads;
		///
		typedef std::deque<input_handle_task_ptr_t> input_task_vec_t;
		input_task_vec_t m_vecInputTasks;
		///
		typedef std::deque<output_handle_task_ptr_t> output_task_vec_t;
		output_task_vec_t m_vecOutputTasks;
		///
		misc_task_ptr_t m_pMiscTask;
	};
	typedef nm_utils::CSmartPtr<nm_framework::CSNEngine> sn_engine_ptr_t;

}

#endif /* __SHARED_OBJ_H__ */
