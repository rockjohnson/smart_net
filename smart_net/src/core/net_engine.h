/*
 * shared_obj.h
 *
 *  Created on: Feb 10, 2011
 *      Author: rock
 */

#ifndef __SHARED_OBJ_H__
#define __SHARED_OBJ_H__

#include <vector>

#include "../common/sn_common.h"
#include "utils/state_machine.h"
#include "sn_endpoint.h"
//#include "sn_proto_wrapper.h"
//#include "../engine/sn_engine.h"


namespace nm_framework
{

/**
 * the smart net manager.
 * */
class CNetEngine: public nm_base::ICommonBase
{
public:
	CNetEngine();
	virtual ~CNetEngine();

	enum E_ENGINE_STATE
	{
		EES_STOPPED = 0, EES_STARTED
	};

	enum E_ENGINE_EVT
	{
		EEE_START = 0, EEE_STOP
	};

public:
	int32_t start(u_int32_t ui32InputThreadCnt,
			u_int32_t ui32OutputThreadCnt,
			int32_t i32IoEvtNotifier,
			int32_t i32MsTimeout);
	int32_t stop();
	///
	int32_t add_endpoint(const endpoint_ptr_t &pEP); ///not thread safe
	int32_t del_endpoint(const endpoint_ptr_t &pEP); ///not thread safe
	int32_t add_timer(const timer_ptr_t &pTimer);
	int32_t del_timer(const timer_ptr_t &pTimer);

private:
	int32_t starting(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, pvoid_t pVoid);
	int32_t stopping(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, pvoid_t pVoid);

private:
	DISALLOW_COPY_AND_ASSIGN(CNetEngine);

	///state
	nm_utils::CStateMachine<CNetEngine> m_sm;
	///threads
	typedef std::vector<nm_thread::thread_ptr_t> thread_vec_t;
	thread_vec_t m_vecThreads;
	///
	typedef std::vector<input_handle_task_ptr_t> input_task_vec_t;
	input_task_vec_t m_vecInputTasks;
	///
	typedef std::vector<output_handle_task_ptr_t> output_task_vec_t;
	output_task_vec_t m_vecOutputTasks;
	///
	misc_task_ptr_t m_pMiscTasks;
};
typedef nm_utils::CSmartPtr<nm_framework::CNetEngine> net_engine_ptr_t;

}

#endif /* __SHARED_OBJ_H__ */
