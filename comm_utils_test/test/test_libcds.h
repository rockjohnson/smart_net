/*
 * test_file.h
 *
 *  Created on: Jan 27, 2011
 *      Author: rock
 */

#ifndef __TEST_LIBCDS_H__
#define __TEST_LIBCDS_H__

#include <cxxtest/TestSuite.h>
#include <cds/intrusive/optimistic_queue.h>
#include <cds/gc/hp.h>

namespace ci = cds::intrusive;
typedef cds::gc::HP hp_gc;

// Optimistic queue with Hazard Pointer garbage collector, base hook + item counter:
struct Foo: public ci::optimistic_queue::node<hp_gc>
{
	// Your data
	//...\

	Foo(int ii): i(ii)
		{

		}

	int i;
};

class CLibcdsTest: public CxxTest::TestSuite
{
public:
	void test_queue()
	{

		cds::Initialize();

		cds::gc::HP hzpGC;
		//cds::gc::HRC hrcGC    ;
		//cds::gc::PTB ptbGC    ;

		cds::gc::hzp::GarbageCollector::instance().setScanType(cds::gc::hzp::classic);

		cds::threading::Manager::attachThread();

		sleep(3);

		ci::OptimisticQueue<Foo> a;
		Foo b(1), c(2);
		a.enqueue(b);
		a.enqueue(c);
		bool bb = a.empty();
		int ii = a.dequeue()->i;
		int j = a.dequeue()->i;
		bb = a.empty();
		std::cout << "optimisticQ size: " << a.size() << std::endl;
		std::cout<<ii << j << std::endl;
	}
};

#endif /* __TEST_FILE_H__ */
