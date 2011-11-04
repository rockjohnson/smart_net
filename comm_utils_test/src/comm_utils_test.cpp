/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/ErrorPrinter.h>

int main() {
 return CxxTest::ErrorPrinter().run();
}
#include "../test/test_containers.h"

static CContainerTest suite_CContainerTest;

static CxxTest::List Tests_CContainerTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_CContainerTest( "../test/test_containers.h", 50, "CContainerTest", suite_CContainerTest, Tests_CContainerTest );

static class TestDescription_CContainerTest_test_one : public CxxTest::RealTestDescription {
public:
 TestDescription_CContainerTest_test_one() : CxxTest::RealTestDescription( Tests_CContainerTest, suiteDescription_CContainerTest, 53, "test_one" ) {}
 void runTest() { suite_CContainerTest.test_one(); }
} testDescription_CContainerTest_test_one;

#include "../test/test_libcds.h"

static CLibcdsTest suite_CLibcdsTest;

static CxxTest::List Tests_CLibcdsTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_CLibcdsTest( "../test/test_libcds.h", 29, "CLibcdsTest", suite_CLibcdsTest, Tests_CLibcdsTest );

static class TestDescription_CLibcdsTest_test_queue : public CxxTest::RealTestDescription {
public:
 TestDescription_CLibcdsTest_test_queue() : CxxTest::RealTestDescription( Tests_CLibcdsTest, suiteDescription_CLibcdsTest, 32, "test_queue" ) {}
 void runTest() { suite_CLibcdsTest.test_queue(); }
} testDescription_CLibcdsTest_test_queue;

#include "../test/test_malloc.h"

static CMallocTest suite_CMallocTest;

static CxxTest::List Tests_CMallocTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_CMallocTest( "../test/test_malloc.h", 15, "CMallocTest", suite_CMallocTest, Tests_CMallocTest );

static class TestDescription_CMallocTest_test_malloc_one : public CxxTest::RealTestDescription {
public:
 TestDescription_CMallocTest_test_malloc_one() : CxxTest::RealTestDescription( Tests_CMallocTest, suiteDescription_CMallocTest, 20, "test_malloc_one" ) {}
 void runTest() { suite_CMallocTest.test_malloc_one(); }
} testDescription_CMallocTest_test_malloc_one;

#include "../test/test_smartlog.h"

static CTestSmartLog suite_CTestSmartLog;

static CxxTest::List Tests_CTestSmartLog = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_CTestSmartLog( "../test/test_smartlog.h", 21, "CTestSmartLog", suite_CTestSmartLog, Tests_CTestSmartLog );

static class TestDescription_CTestSmartLog_test_log : public CxxTest::RealTestDescription {
public:
 TestDescription_CTestSmartLog_test_log() : CxxTest::RealTestDescription( Tests_CTestSmartLog, suiteDescription_CTestSmartLog, 24, "test_log" ) {}
 void runTest() { suite_CTestSmartLog.test_log(); }
} testDescription_CTestSmartLog_test_log;

#include <cxxtest/Root.cpp>
