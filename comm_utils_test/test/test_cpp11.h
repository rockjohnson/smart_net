/*
 * test_cpp11.h
 *
 *  Created on: Sep 10, 2011
 *      Author: rock
 */

#ifndef TEST_CPP11_H_
#define TEST_CPP11_H_

#if 0
#include <cxxtest/TestSuite.h>
#include <iostream>
#include <vector>
#include <string>
#include <typeinfo>
#include <atomic>



constexpr int get_five()
{
	return 5;
}

int some_value[get_five() + 7]; //create an array of 12 integers. Illegal C++

///////////////////--test-determine-function-return-value--//////////////
struct Clear
{
	double operator()(const int32_t) const// The parameter type is
	{
		return 0.00;
	}

	int32_t operator()(const double) const// equal to the return type.
	{
		return 0;
	}
};

template<class T1>
class Calculus
{
public:
	template<class T2>
	typename std::result_of<T1(T2)>::type operator()(const T2& t2) const
	{
		return t1(t2);
	}
private:
	T1 t1;
};

///////////////////--test-determine-function-return-value--//////////////

template<int B, int N>
struct Pow
{
	// recursive call and recombination.
	enum
	{
		value = B * Pow<B, N - 1>::value
	};
};

template<int B>
struct Pow<B, 0>
{
	// ''N == 0'' condition of termination.
	enum
	{
		value = 1
	};
};

// First way of operating.
template<bool B> struct Algorithm
{
	template<class T1, class T2>
	static int do_it(T1 &, T2 &)
	{ /*...*/
		return 0;
	}
};

// Second way of operating.
template<>
struct Algorithm<true>
{
	template<class T1, class T2>
	static int do_it(T1, T2)
	{ /*...*/
		return 0;
	}
};

// Instantiating 'elaborate' will automatically instantiate the correct way to operate.
template<class T1, class T2>
int elaborate(T1 A, T2 B)
{
	// Use the second way only if 'T1' is an integer and if 'T2' is
	// in floating point, otherwise use the first way.
	return Algorithm < std::is_integral<T1>::value
			&& std::is_floating_point<T2>::value > ::do_it(A, B);
}

//#include <type_traits>
///////////////////////////////////////////////////////////////////

#include <functional>

int32_t my_func(int32_t i, int32_t j)
{
	return i + j;
}

class CTest01
{
public:
	int32_t run(int32_t, int32_t)
	{
		std::cout << "run!" << std::endl;
		return 0;
	}
};

////////////////////////////////////////////////////////////////////////

// This function will obtain a reference to the parameter 'r' and increment it.
void f(int &r)
{
	r++;
}

// Template function.
template<class F, class P> void g(F f, P t)
{
	f(t);
}

///////////////////////////////////////////////////////////////////////

#include <random>
#include <functional>
/////////////////////////////////////

#include <memory>

/////////////////////////////////////

class CCPPTest:
{
public:

	void test_pp()
	{
		std::cout << u"ok!" <<std::endl;

		std::vector<std::string> vec =
		{	"1", "2", "3"};
		auto vec1 =
		{	"1", "2"};
		std::cout<<typeid(vec1).name()<<std::endl;

		auto tmp = "string";
		std::cout<<tmp<<typeid(tmp).name()<<std::endl;

		for (std::string &x : vec)
		{
			std::cout<<x<<std::endl;
		}
	}

	void test_atomic()
	{
		std::atomic_int i32Test;
		std::atomic_int_fast32_t t;
		t++;
		i32Test = 0;
		std::atomic<int> ati;
		ati++;
	}

	void test_determine_function_return_val()
	{
		Calculus<Clear> cl;
		std::cout<< "the result of Calculus 0 is : "<< cl(1)<< std::endl;
	}

	void test_type_traits()
	{
		int quartic_of_three = Pow<3, 4>::value;
		std::cout<< "the value of quartic is : "<< quartic_of_three<<std::endl;

		std::cout<< elaborate(1, 2)<< std::endl;
	}

	int32_t my_func1(int32_t, int32_t)
	{
		std::cout<< "haha"<< std::endl;
	}

	void test_function_wrapper()
	{
		std::function<int (int, int)> func; // Wrapper creation using
				func = &my_func;
				// template class 'function'.
				//std::plus<int> add; // 'plus' is declared as 'template<class T> T plus( T, T ) ;'
				// then 'add' is type 'int add( int x, int y )'.
				//func = &add; // OK - Parameters and return types are the same.

				//int a = func (1, 2); // NOTE: if the wrapper 'func' does not refer to any function,
				// the exception 'std::bad_function_call' is thrown.
				std::cout<< "the func result is : " << func(1, 2) << std::endl;
				CTest01 tt;
				func = std::bind(&CTest01::run, &tt, std::placeholders::_1, std::placeholders::_2);
				func(2, 3);

				std::function<bool (short, short)> func2;
				if(!func2)
				{ // True because 'func2' has not yet been assigned a function.

					//bool adjacent(long x, long y);
					//func2 = &adjacent; // OK - Parameters and return types are convertible.

					struct Test
					{
						bool operator()(short x, short y)
						{
							return true;
						}
					};
					Test car;
					func2 = std::ref(car); // 'std::ref' is a template function that returns the wrapper
					// of member function 'operator()' of struct 'car'.
				}
				func = func2; // OK - Parameters and return types are convertible.
			}

			void test_wrapper_reference()
			{
				int i = 0;
				g (f, i); // 'g<void (int &r), int>' is instantiated
				// then 'i' will not be modified.
				std::cout << i << std::endl; // Output -> 0

				g (f, std::ref(i)); // 'g<void(int &r),reference_wrapper<int>>' is instantiated
				// then 'i' will be modified.
				std::cout << i << std::endl; // Output -> 1

			}

			void test_presudorandom()
			{
				std::uniform_int_distribution<int> distribution(0, 99);
				std::mt19937 engine; // Mersenne twister MT19937
				auto generator = std::bind(distribution, engine);
				int random = generator(); // Generate a uniform integral variate between 0 and 99.
				int random2 = distribution(engine); // Generate another sample directly using the distribution and the engine objects.

				std::cout<< random<< random2<< std::endl;
			}

			///General-purpose smart pointers
			void test_smart_pointer()
			{
				class CTmp
				{
				public:
					CTmp()
					{
						std::cout<< "construct"<<std::endl;
					}

					~CTmp()
					{
						std::cout<< "destruct"<< std::endl;
					}
				};
				std::shared_ptr<CTmp> ptr(new CTmp());
				std::shared_ptr<CTmp> pp = ptr;

				ptr = NULL;
				pp = NULL;

				std::unique_ptr<int> p1(new int(5));
				//p1 = new int(5);
				//std::unique_ptr<int> p2 = p1; //Compile error.
				std::unique_ptr<int> p3 = std::move(p1); //Transfers ownership. p3 now owns the memory and p1 is rendered invalid.

				p3.reset(); //Deletes the memory.
				p1.reset(); //Does nothing.

				std::shared_ptr<int> p9(new int(5));
				//p9 = new int(5);
				std::shared_ptr<int> p8 = p9; //Both now own the memory.

				p9.reset(); //Memory still exists, due to p2.
				p8.reset(); //Deletes the memory, since no one else owns the memory.

				std::shared_ptr<int> p4(new int(5));
				std::weak_ptr<int> wp1 = p4; //p1 owns the memory.

				{
				  std::shared_ptr<int> p5 = wp1.lock(); //Now p1 and p2 own the memory.
				  if(p5) //Always check to see if the memory still exists
				  {
				    //Do something with p2
				  }
				} //p2 is destroyed. Memory is owned by p1.

				p4.reset(); //Memory is deleted.

				std::shared_ptr<int> p6 = wp1.lock(); //Memory is gone, so we get an empty shared_ptr.
				if(p6)
				{
				  //Will not execute this.
				}

			}
		};

#endif
#endif /* TEST_CPP11_H_ */
