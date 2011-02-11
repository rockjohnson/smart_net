/*
 * test_file.h
 *
 *  Created on: Jan 27, 2011
 *      Author: rock
 */

#ifndef __TEST_FILE_H__
#define __TEST_FILE_H__

#include <cxxtest/TestSuite.h>
#include <utils/file.h>

class CFileTest: public CxxTest::TestSuite
{
	public:
		void test_create()
		{
			nm_utils::CLiteFile f;

			TS_ASSERT(RET_SUC == f.create(string_t("test/test.log")));
		}
};

#endif /* __TEST_FILE_H__ */
