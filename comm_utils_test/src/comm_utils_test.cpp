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
#include "../test/test_file.h"

static CFileTest suite_CFileTest;

static CxxTest::List Tests_CFileTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_CFileTest( "../test/test_file.h", 14, "CFileTest", suite_CFileTest, Tests_CFileTest );

static class TestDescription_CFileTest_test_create : public CxxTest::RealTestDescription {
public:
 TestDescription_CFileTest_test_create() : CxxTest::RealTestDescription( Tests_CFileTest, suiteDescription_CFileTest, 17, "test_create" ) {}
 void runTest() { suite_CFileTest.test_create(); }
} testDescription_CFileTest_test_create;

#include "../test/test_smartlog.h"

static CTestSmartLog suite_CTestSmartLog;

static CxxTest::List Tests_CTestSmartLog = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_CTestSmartLog( "../test/test_smartlog.h", 15, "CTestSmartLog", suite_CTestSmartLog, Tests_CTestSmartLog );

static class TestDescription_CTestSmartLog_test_log : public CxxTest::RealTestDescription {
public:
 TestDescription_CTestSmartLog_test_log() : CxxTest::RealTestDescription( Tests_CTestSmartLog, suiteDescription_CTestSmartLog, 18, "test_log" ) {}
 void runTest() { suite_CTestSmartLog.test_log(); }
} testDescription_CTestSmartLog_test_log;

#include "../test/thread_test.h"

static CThreadTest suite_CThreadTest;

static CxxTest::List Tests_CThreadTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_CThreadTest( "../test/thread_test.h", 33, "CThreadTest", suite_CThreadTest, Tests_CThreadTest );

static class TestDescription_CThreadTest_test_thread : public CxxTest::RealTestDescription {
public:
 TestDescription_CThreadTest_test_thread() : CxxTest::RealTestDescription( Tests_CThreadTest, suiteDescription_CThreadTest, 37, "test_thread" ) {}
 void runTest() { suite_CThreadTest.test_thread(); }
} testDescription_CThreadTest_test_thread;

#include <cxxtest/Root.cpp>
