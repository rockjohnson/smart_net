/*
 * test_smartlog.h
 *
 *  Created on: Jan 14, 2011
 *      Author: rock
 */

#ifndef __TEST_SMARTLOG_H__
#define __TEST_SMARTLOG_H__

#include <cxxtest/TestSuite.h>

#include <log/smart_log.h>

class CTestSmartLog : public CxxTest::TestSuite
{
	public:
		void test_log()
		{
			nm_utils::CSmartLog log;
			log.init("./test_smartlog/haha/", "smartlog_", nm_utils::ELL_DEBUG, 30);

			TRACE_LOG(log, nm_utils::ELL_ERR, "this is a error test!\n");
		}

//		void test_detail_log()
//		{
//			nm_utils::CSmartLog log;
//			log.init("./test_smartlog/", "smartlogdetail_", nm_utils::ELL_DEBUG, 30);
//
//			LOG_DETAIL(log, nm_utils::ERR_ERR, "this is a detail error test!\n");
//		}
};



#endif /* __TEST_SMARTLOG_H__ */
