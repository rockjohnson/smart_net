/*
 * thread_test.h
 *
 *  Created on: Dec 30, 2010
 *      Author: rock
 */

#ifndef _THREAD_TEST_H___
#define _THREAD_TEST_H___

#include <cxxtest/TestSuite.h>

#if (__TEST_MY_THREAD__)

#include <thread/thread_ex.h>
#include <utils/utils.h>
#include <log/smart_log.h>

nm_utils::CSmartLog mylog;

class CMyTask : public nm_thread::IThreadTask
{
public:
	void exec()
	{
		mylog.init("thread/test/", "thread_test", nm_utils::ELL_DEF, 5);
		while (!is_stopped())
		{
			nm_utils::sleep(1, 0);
			TRACE_LOG(mylog, nm_utils::ELL_DEBUG, "test: %d\n", 5);
		}
	}
};

class CThreadTest
{

public:
	void test_thread()
	{
		nm_thread::CThread thread;
		nm_thread::thread_task_ptr_t pTask = new CMyTask;

		thread.assign_task(pTask);
		thread.start();
		nm_utils::sleep(10, 0);
		thread.stop_wait();

		TRACE_LOG(mylog, nm_utils::ELL_ERR, "err: %d\n", 1);
	}
};


//class MyTestSuite :
//{
//public:
//void testAddition( void )
//{
//TS_ASSERT( 1 + 1 > 1 );
//TS_ASSERT_EQUALS( 1 + 1, 2 );
//}
//};

#elif (__TEST_STD_THREAD__)

#include <thread>

class CStdThreadTest :
{

public:

	void test_thread()
	{
		std::thread thrd;
	}
};

#endif

#endif /* __THREAD_TEST_H__ */
