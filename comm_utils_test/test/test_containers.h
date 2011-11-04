/*
 * test_containers.h
 *
 *  Created on: Oct 21, 2011
 *      Author: rock
 */

#ifndef TEST_CONTAINERS_H_
#define TEST_CONTAINERS_H_

#include <set>
#include <vector>
#include <deque>
#include <iostream>
#include <google/dense_hash_map>
#include <string>

class CTestOne
{
public:
	CTestOne()
	{
		std::cout << "CTestOne" << std::endl;
	}

	CTestOne(const CTestOne &)
	{
		std::cout << "CTestOne&" << std::endl;
	}

	CTestOne& operator =(const CTestOne &)
	{
		std::cout << "CTestOne =" << std::endl;
	}



	~CTestOne()
	{
		std::cout << "~CTestOne" << std::endl;
	}
};

bool operator < (const CTestOne &a, const CTestOne &b)
{
	std::cout << "CTestOne <" << std::endl;
	return false;
}

class CContainerTest: public CxxTest::TestSuite
{
public:
	void test_one()
	{
		google::dense_hash_map<std::string, int> b;
		b.set_empty_key(std::string(""));
		std::string str("aaa");
		b[str] = 1;
		int c = b[str];

		//std::set<CTestOne> s;
		std::deque<CTestOne> s;
		CTestOne one;
		s.push_back(one);
	}

};

#endif /* TEST_CONTAINERS_H_ */
