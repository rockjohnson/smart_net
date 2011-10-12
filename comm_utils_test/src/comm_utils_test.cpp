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
CxxTest::StaticSuiteDescription suiteDescription_CCPPTest( "../test/test_cpp11.h", 149, "CCPPTest", suite_CCPPTest, Tests_CCPPTest );

static class TestDescription_CCPPTest_test_pp : public CxxTest::RealTestDescription {
public:
 TestDescription_CCPPTest_test_pp() : CxxTest::RealTestDescription( Tests_CCPPTest, suiteDescription_CCPPTest, 153, "test_pp" ) {}
 void runTest() { suite_CCPPTest.test_pp(); }
} testDescription_CCPPTest_test_pp;

static class TestDescription_CCPPTest_test_atomic : public CxxTest::RealTestDescription {
public:
 TestDescription_CCPPTest_test_atomic() : CxxTest::RealTestDescription( Tests_CCPPTest, suiteDescription_CCPPTest, 172, "test_atomic" ) {}
 void runTest() { suite_CCPPTest.test_atomic(); }
} testDescription_CCPPTest_test_atomic;

static class TestDescription_CCPPTest_test_determine_function_return_val : public CxxTest::RealTestDescription {
public:
 TestDescription_CCPPTest_test_determine_function_return_val() : CxxTest::RealTestDescription( Tests_CCPPTest, suiteDescription_CCPPTest, 182, "test_determine_function_return_val" ) {}
 void runTest() { suite_CCPPTest.test_determine_function_return_val(); }
} testDescription_CCPPTest_test_determine_function_return_val;

static class TestDescription_CCPPTest_test_type_traits : public CxxTest::RealTestDescription {
public:
 TestDescription_CCPPTest_test_type_traits() : CxxTest::RealTestDescription( Tests_CCPPTest, suiteDescription_CCPPTest, 188, "test_type_traits" ) {}
 void runTest() { suite_CCPPTest.test_type_traits(); }
} testDescription_CCPPTest_test_type_traits;

static class TestDescription_CCPPTest_test_function_wrapper : public CxxTest::RealTestDescription {
public:
 TestDescription_CCPPTest_test_function_wrapper() : CxxTest::RealTestDescription( Tests_CCPPTest, suiteDescription_CCPPTest, 201, "test_function_wrapper" ) {}
 void runTest() { suite_CCPPTest.test_function_wrapper(); }
} testDescription_CCPPTest_test_function_wrapper;

static class TestDescription_CCPPTest_test_wrapper_reference : public CxxTest::RealTestDescription {
public:
 TestDescription_CCPPTest_test_wrapper_reference() : CxxTest::RealTestDescription( Tests_CCPPTest, suiteDescription_CCPPTest, 238, "test_wrapper_reference" ) {}
 void runTest() { suite_CCPPTest.test_wrapper_reference(); }
} testDescription_CCPPTest_test_wrapper_reference;

static class TestDescription_CCPPTest_test_presudorandom : public CxxTest::RealTestDescription {
public:
 TestDescription_CCPPTest_test_presudorandom() : CxxTest::RealTestDescription( Tests_CCPPTest, suiteDescription_CCPPTest, 251, "test_presudorandom" ) {}
 void runTest() { suite_CCPPTest.test_presudorandom(); }
} testDescription_CCPPTest_test_presudorandom;

static class TestDescription_CCPPTest_test_smart_pointer : public CxxTest::RealTestDescription {
public:
 TestDescription_CCPPTest_test_smart_pointer() : CxxTest::RealTestDescription( Tests_CCPPTest, suiteDescription_CCPPTest, 263, "test_smart_pointer" ) {}
 void runTest() { suite_CCPPTest.test_smart_pointer(); }
} testDescription_CCPPTest_test_smart_pointer;

#include "../test/test_file.h"

static CFileTest suite_CFileTest;

static CxxTest::List Tests_CFileTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_CFileTest( "../test/test_file.h", 14, "CFileTest", suite_CFileTest, Tests_CFileTest );

static class TestDescription_CFileTest_test_create : public CxxTest::RealTestDescription {
public:
 TestDescription_CFileTest_test_create() : CxxTest::RealTestDescription( Tests_CFileTest, suiteDescription_CFileTest, 17, "test_create" ) {}
 void runTest() { suite_CFileTest.test_create(); }
} testDescription_CFileTest_test_create;

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
