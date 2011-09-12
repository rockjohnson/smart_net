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
#include "../test/test_cpp11.h"

static CCPPTest suite_CCPPTest;

static CxxTest::List Tests_CCPPTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_CCPPTest( "../test/test_cpp11.h", 21, "CCPPTest", suite_CCPPTest, Tests_CCPPTest );

static class TestDescription_CCPPTest_test_pp : public CxxTest::RealTestDescription {
public:
 TestDescription_CCPPTest_test_pp() : CxxTest::RealTestDescription( Tests_CCPPTest, suiteDescription_CCPPTest, 25, "test_pp" ) {}
 void runTest() { suite_CCPPTest.test_pp(); }
} testDescription_CCPPTest_test_pp;

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
CxxTest::StaticSuiteDescription suiteDescription_CTestSmartLog( "../test/test_smartlog.h", 21, "CTestSmartLog", suite_CTestSmartLog, Tests_CTestSmartLog );

static class TestDescription_CTestSmartLog_test_log : public CxxTest::RealTestDescription {
public:
 TestDescription_CTestSmartLog_test_log() : CxxTest::RealTestDescription( Tests_CTestSmartLog, suiteDescription_CTestSmartLog, 24, "test_log" ) {}
 void runTest() { suite_CTestSmartLog.test_log(); }
} testDescription_CTestSmartLog_test_log;

#include "../test/thread_test.h"

static CStdThreadTest suite_CStdThreadTest;

static CxxTest::List Tests_CStdThreadTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_CStdThreadTest( "../test/thread_test.h", 70, "CStdThreadTest", suite_CStdThreadTest, Tests_CStdThreadTest );

static class TestDescription_CStdThreadTest_test_thread : public CxxTest::RealTestDescription {
public:
 TestDescription_CStdThreadTest_test_thread() : CxxTest::RealTestDescription( Tests_CStdThreadTest, suiteDescription_CStdThreadTest, 75, "test_thread" ) {}
 void runTest() { suite_CStdThreadTest.test_thread(); }
} testDescription_CStdThreadTest_test_thread;

#include <cxxtest/Root.cpp>
