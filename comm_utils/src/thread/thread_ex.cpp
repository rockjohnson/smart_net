/*
 * thread_ex.cpp
 *
 *  Created on: Dec 29, 2010
 *      Author: rock
 */

#include "thread_ex.h"
//
#include <pthread.h>
//
#include "../utils/utils.h"
#include "../log/smart_log.h"

#define __THREAD_SLEEP_INTERVAL__ (1) //secs
namespace nm_thread
{

IThreadTask::IThreadTask() :
	m_bStop(false)
{
}

IThreadTask::~IThreadTask()
{
}

void IThreadTask::stop()
{
	LOG_ASSERT(!is_stopped());

	m_bStop = true;
}

bool IThreadTask::is_stopped()
{
	return m_bStop;
}

/**
 * CThread
 * */
cmn_pvoid_t CThread::thread_main(cmn_pvoid_t pArg)
{
	if (NULL == pArg)
	{
		LOG_ASSERT(false);
		return NULL;
	}

	CThread *pThread = static_cast<CThread*> (pArg);

	return pThread->exec();
}

CThread::CThread() :
	m_bStop(false), m_thread(0)
{
}

CThread::~CThread()
{
	destroy();
}

int32_t CThread::init()
{
	SYS_ASSERT(NULL == m_pTask);
#if 0
	m_pTask = NULL;
#endif
	m_bStop = false;
	m_bRunning = false;
#if __PLATFORM__ == __PLATFORM_LINUX__
	m_thread = 0;
	int32_t i32Ret = pthread_attr_init(&m_attr); ///The  pthread_attr_init()  function  initializes  the  thread attributes object pointed to by attr with default attribute values.
#elif __PLATFORM__ == __PLATFORM_LINUX__
#endif
	return i32Ret;
}

int32_t CThread::destroy()
{
	SYS_ASSERT(m_bStop);
	SYS_ASSERT(!m_bRunning);
	m_pTask = NULL;
#if __PLATFORM__ == __PLATFORM_LINUX__
#if 0
	m_thread = 0;
#endif
	int32_t i32Ret = pthread_attr_destroy(&m_attr); ///The  pthread_attr_init()  function  initializes  the  thread attributes object pointed to by attr with default attribute values.
#else
#endif
	return i32Ret;
}

int32_t CThread::start()
{
	if (NULL == m_pTask)
	{
		return RET_ERR;
	}

	return pthread_create(&m_thread, &m_attr, &thread_main, this);
}

int32_t CThread::stop()
{
	if (NULL != m_pTask)
	{
		m_pTask->stop();
	}

	LOG_ASSERT(!m_bStop);
	m_bStop = true;

	return RET_SUC;
}

int32_t CThread::stop_wait()
{
	stop();

#if defined(__PLATEFORM_LINUX__)
	return pthread_join(m_thread, NULL);
#else
#endif
	reset_task();

	return RET_SUC;
}

bool CThread::is_running()
{
	return m_bRunning;
}

thread_task_ptr_t& CThread::get_task()
{
	return m_pTask;
}

int32_t CThread::assign_task(const thread_task_ptr_t &pTask)
{
	SYS_ASSERT(NULL == m_pTask);
	m_pTask = pTask;

	return RET_SUC;
}

int32_t CThread::reset_task()
{
	SYS_ASSERT(NULL != m_pTask);
	m_pTask = NULL;

	return RET_SUC;
}

void CThread::enter_run()
{
	m_bRunning = true;
}

void CThread::leave_run()
{
	m_bRunning = false;
}

bool CThread::is_stopped()
{
	return m_bStop;
}

cmn_pvoid_t CThread::exec()
{
	enter_run();

	while (!is_stopped())
	{
		LOG_ASSERT(NULL != m_pTask);
		m_pTask->exec();

		nm_utils::sleep(__THREAD_SLEEP_INTERVAL__, 0);
	}

	leave_run();

	return NULL;
}

}
