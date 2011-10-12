/*
 * test_malloc.h
 *
 *  Created on: Oct 12, 2011
 *      Author: rock
 */

#ifndef TEST_MALLOC_H_
#define TEST_MALLOC_H_

//#include <stdlib.h>
//#include <jemalloc/jemalloc.h>
#include <cxxtest/TestSuite.h>

class CMallocTest: public CxxTest::TestSuite
{

public:

	void test_malloc_one()
	{
		void *pV = malloc(1024); ///经过测试发现，链接程序的时候jemalloc和tcmalloc哪个放在前面，malloc的实现体就是那个版本
	}
};

#endif /* TEST_MALLOC_H_ */
