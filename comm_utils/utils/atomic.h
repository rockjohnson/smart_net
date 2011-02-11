/*
 * atomic.h
 *
 *  Created on: May 20, 2009
 *      Author: rock
 */

#ifndef __ATOMIC_H__
#define __ATOMIC_H__

#include "../common/common.h"

#if __PLATFORM__ == __PLATFORM_WINDOWS__
#include <Windows.h>
#include <intrin.h>
#endif

#ifdef __USING_GNU_ATOMIC__
#include <ext/atomicity.h>
#endif

namespace nm_utils
{

	inline int32_t atomic_exchange_and_add(int32_t *pw, int32_t dv)
	{
#if __PLATFORM__ == __PLATFORM_WINDOWS__
		//return InterlockedExchangeAddAcquire(pw, dv);
		return InterlockedExchangeAdd((volatile long*)pw, dv);
#elif __PLATFORM__ == __PLATFORM_LINUX__

#ifdef __USING_GNU_ATOMIC__
		return __gnu_cxx::__exchange_and_add_dispatch(pw, dv);
#else
		// int r = *pw;
		// *pw += dv;
		// return r;

		int r;

		__asm__ __volatile__
		(
				"lock\n\t"
				"xadd %1, %0":
				"=m"( *pw ), "=r"( r ): // outputs (%0, %1)
				"m"( *pw ), "1"( dv ): // inputs (%2, %3 == %1)
				"memory", "cc" // clobbers
		);

		return r;
#endif
#endif
	}

	inline void atomic_increment(int32_t *pw)
	{
#if __PLATFORM__ == __PLATFORM_WINDOWS__
		::InterlockedIncrementAcquire((volatile long*)pw);
		//InterlockedIncrementRelease(pw);
#elif __PLATFORM__ == __PLATFORM_LINUX__

#ifdef __USING_GNU_ATOMIC__
		__gnu_cxx::__atomic_add_dispatch(pw, 1);
#else
		//atomic_exchange_and_add( pw, 1 );

		__asm__
		(
				"lock\n\t"
				"incl %0":
				"=m"( *pw ): // output (%0)
				"m"( *pw ): // input (%1)
				"cc" // clobbers
		);
#endif
#endif
	}

#if 0
	inline int atomic_conditional_increment(int * pw)
	{
		// int rv = *pw;
		// if( rv != 0 ) ++*pw;
		// return rv;

		int rv, tmp;

		__asm__
		(
				"movl %0, %%eax\n\t"
				"0:\n\t"
				"test %%eax, %%eax\n\t"
				"je 1f\n\t"
				"movl %%eax, %2\n\t"
				"incl %2\n\t"
				"lock\n\t"
				"cmpxchgl %2, %0\n\t"
				"jne 0b\n\t"
				"1:":
				"=m"( *pw ), "=&a"( rv ), "=&r"( tmp ): // outputs (%0, %1, %2)
				"m"( *pw ): // input (%3)
				"cc" // clobbers
		);

		return rv;
	}
#endif

	///////////////////////////////////////////
	template <typename T>
	class CAtomicCnt
	{
	public:
		CAtomicCnt(T cnt = 0)
		:m_cnt(cnt)
		{
		}

		CAtomicCnt(const CAtomicCnt &other)
		:m_cnt(other.m_cnt)
		{
		}

		CAtomicCnt& operator=(const CAtomicCnt &other)
		{
			m_cnt = other.m_cnt;
			return *this;
		}

		bool operator==(const CAtomicCnt &other)
		{
			return m_cnt == other.m_cnt;
		}

		T operator++()
		{
			return atomic_exchange_and_add(&m_cnt, +1) + 1;
		}
		T operator++(int)
		{
			return atomic_exchange_and_add(&m_cnt, +1);
		}
		T operator--()
		{
			return atomic_exchange_and_add(&m_cnt, -1) - 1;
		}
		T operator--(int)
		{
			return atomic_exchange_and_add(&m_cnt, -1);
		}

		operator T()
		{
			return m_cnt;
		}


	private:
		T m_cnt;
	};

} // namespace nm_utils


#endif /* __ATOMIC_H__ */
