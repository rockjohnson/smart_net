/*
 * thread.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef THREAD_H_
#define THREAD_H_

#include "../common/common.h"
#include "../utils/state_machine.h"

namespace nm_thread
{
/**
 * thread wrapper...
 * */
template <class THREAD_TASK>
class CThread
{
	typedef nm_utils::CSmartPtr<THREAD_TASK> task_ptr_t;

	enum EEvt
	{
		EE_START = 0,
		EE_STOP
	};

	enum EState
	{
		ES_STOPPED = 0,
		ES_STARTING,
		ES_RUNNING,
		ES_STOPPING,
	};

public:
	CThread();
	~CThread();

public:
	int start();
	int stop();
	task_ptr_t& get_task();
	int get_state();


private:
	task_ptr_t m_pTask;
	nm_utils::CStateMachine<nm_thread::CThread<THREAD_TASK> > m_sm;
};

#include "thread.inl"

}

#endif /* THREAD_H_ */
