/*
 * time_info.cpp
 *
 *  Created on: Apr 3, 2009
 *      Author: rock
 */

#include "time_info.h"

#if __PLATFORM__ == __PLATFORM_LINUX__
#define _ISOC99_SOURCE
#define _POSIX_SOURCE

#include <sys/time.h>
#elif __PLATFORM__ == __PLATFORM_WINDOWS__
#include <Windows.h>
#endif
#include <wchar.h>
#include <time.h>

namespace nm_utils
{

CTimeInfo::CTimeInfo()
{
	// TODO Auto-generated constructor stub

}

CTimeInfo::~CTimeInfo()
{
	// TODO Auto-generated destructor stub
}

time_t CTimeInfo::get_time_in_seconds()
{
	return time(NULL);
}

#define __MICRO_SECOND__
const char_t* CTimeInfo::get_day_time(char_t *pszBuf)
{
#if __PLATFORM__ == __PLATFORM_LINUX__
#ifdef __MICRO_SECOND__
	struct timeval tv;
	gettimeofday(&tv, NULL);
#else
	time_t tt = get_time_in_seconds();
#endif

	struct tm t;
#if defined __MICRO_SECOND__
	localtime_r(&(tv.tv_sec), &t);
	sprintf(pszBuf, 36, L"%i-%2.2i-%2.2i_%2.2i:%2.2i:%2.2i.%6.6ld", t.tm_year
			+ 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec,
			tv.tv_usec);
#else
	localtime_r(&tt, &t);
	swprintf(pwszBuf, 36, L"%i-%2.2i-%2.2i %2.2i:%2.2i:%2.2i",
			t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
#endif
#elif defined(__PLATEFORM_WINDOWS__)
	SYSTEMTIME st;
	::GetLocalTime(&st);

	::swprintf(pwszBuf, L"%4.4u-%2.2u-%2.2u(%2.2u:%2.2u:%2.2u.%3.3u)",
			st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
#endif

	return pszBuf;
}

u_int64_t CTimeInfo::get_current_time_us()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	return tv.tv_sec * (1000 * 1000) + tv.tv_usec;
}

}
