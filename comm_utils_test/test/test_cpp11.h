/*
 * test_cpp11.h
 *
 *  Created on: Sep 10, 2011
 *      Author: rock
 */

#ifndef TEST_CPP11_H_
#define TEST_CPP11_H_

#include <cxxtest/TestSuite.h>
#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>

constexpr int get_five() {return 5;}

int some_value[get_five() + 7]; //create an array of 12 integers. Illegal C++

class CCPPTest : public CxxTest::TestSuite
{
public:

	void test_pp()
	{
		std::cout<< u"ok!" <<std::endl;

		std::vector<std::string> vec = {"1", "2", "3"};
		auto vec1 = {"1", "2"};
		std::cout<<typeid(vec1).name()<<std::endl;

		auto tmp = "string";
		std::cout<<tmp<<typeid(tmp).name()<<std::endl;

		for (std::string &x : vec)
		{
			std::cout<<x<<std::endl;
		}

	}
};



#endif /* TEST_CPP11_H_ */
