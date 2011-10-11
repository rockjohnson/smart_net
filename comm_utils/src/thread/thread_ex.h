/*
 * thread_ex.h
 *
 *  Created on: Dec 29, 2010
 *      Author: rock
 */

#ifndef __THREAD_EX_H__
#define __THREAD_EX_H__

#include "../common/common.h"
#include "../utils/smart_ptr.h"

namespace nm_thread
{

/**
 * thread task base
 * */
class IThreadTask: public nm_cmn_base::ICommonBase
{
public:
	IThreadTask();
	virtual ~IThreadTask();

public:
	virtual void exec() = 0;
	virtual void stop();

protected:
	virtual bool is_stopped();

protected:
	volatile bool m_bStop; ///stop flag
};
typedef nm_utils::CSmartPtr<nm_thread::IThreadTask> thread_task_ptr_t;

/**
 * common thread
 * */
class CThread : public nm_cmn_base::ICommonBase
{
public:
	enum EEvt
	{
		EE_START = 0, EE_PAUSE, ///not imp..
		EE_STOP
	};

	enum EState
	{
		ES_STOPPED = 0, ES_STARTING, ES_RUNNING, ES_STOPPING,
	};

public:
	CThread();
	~CThread();

public:
	int32_t init();
	int32_t assign_task(const thread_task_ptr_t &pTask);
	int32_t start();
	int32_t pause(); ///not imp..
	int32_t stop();
	int32_t stop_wait();
	int32_t stop_task();
	int32_t reset_task();
	int32_t destroy();
	bool is_running();
	thread_task_ptr_t& get_task();

private:
	cmn_pvoid_t exec();
	bool is_stopped();
	void enter_run();
	void leave_run();

	static cmn_pvoid_t thread_main(cmn_pvoid_t pArg);

private:
	thread_task_ptr_t m_pTask;
	volatile bool m_bStop;
	volatile bool m_bRunning;

#if __PLATFORM__ == __PLATFORM_LINUX__
	pthread_attr_t m_attr;
	pthread_t m_thread;
#elif __PLATFORM__ == __PLATFORM_WINDOWS__
#else
#endif

};
typedef nm_utils::CSmartPtr<nm_thread::CThread> thread_ptr_t;

}

#endif /* __THREAD_EX_H__ */
