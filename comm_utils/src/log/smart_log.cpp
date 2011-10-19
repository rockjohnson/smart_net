/*
 * log.cpp
 *
 *  Created on: Apr 3, 2009
 *      Author: rock
 */

#include "smart_log.h"

#if defined(__PLATEFORM_LINUX__)
#include <unistd.h>
#include <sys/syscall.h>
#elif defined(__PLATEFORM_WINDOWS__)
#include <time.h>
#endif
#include <sys/types.h>
#include <stdio.h>
#include <string>
#include <stdarg.h>

#include "../utils/utils.h"
//#if defined(__PLATEFORM_LINUX__)
//#define OPEN_MODE "a"
//#elif defined(__PLATEFORM_WINDOWS__)
//#define OPEN_MODE "a, ccs=UTF-8"
//#endif
#define LOG_TEMP_BUF_LEN (8192)
//#define DEFAULT_LOG "default.log"
//#define __OUTPUT_TO_CONSOLE__ (1)


namespace nm_utils
{
	namespace
	{
		const cmn_char_t *g_arrLevInfo[ELL_ALL] =
		{	"DEFAULT" , "DEBUG  ", "INFOR  ", "WARNING", "ERROR  "};
	}

	CSmartLog::CSmartLog()
	: m_i32TraceLevel(ELL_ERR), m_tmLogFileStart(0), m_i32IntervalInSeconds(60)
	{
	}

	CSmartLog::~CSmartLog()
	{
		close(true);
	}

	int32_t CSmartLog::init(cmn_cstr_t pcszLogDir, cmn_cstr_t pcszLogFilePrefix, int32_t i32LogLevel, int32_t i32IntervalInSeconds)
	{
		CMN_ASSERT(!m_LogFile.is_open() && 0 == m_tmLogFileStart);

		m_strLogDir = pcszLogDir;
		m_strLogFilePrefix = pcszLogFilePrefix;
		m_i32TraceLevel = i32LogLevel;
		m_i32IntervalInSeconds = i32IntervalInSeconds;

		return RET_SUC;
	}

//	int CSmartLog::init_log(cstr_t pcszLogFile)
//	{
//		assert(pcszLogFile != NULL /*&& m_pLogFile == NULL*/);
//		//m_i32TraceLevel = iLogLevel;
//		m_pLogFile = ::fopen(pcszLogFile, OPEN_MODE);
//#if defined(__PLATEFORM_LINUX__)
//		assert(m_pLogFile != NULL && fwide(m_pLogFile, 0) < 0);
//#elif defined(__PLATEFORM_WINDOWS__)
//		assert(m_pLogFile != NULL);
//#endif
//		return m_pLogFile == NULL ? -1 : RET_SUC;
//	}

	int32_t CSmartLog::check_log_file()
	{
		if ((m_LogFile.is_open()) && ((ZERO >= m_i32IntervalInSeconds) || (time(NULL) < (m_tmLogFileStart + m_i32IntervalInSeconds))))
		{
			return RET_SUC;
		}

		{
			CScopeLock<CMutexLock> lock(m_lock);

			if ((m_LogFile.is_open()) && ((ZERO >= m_i32IntervalInSeconds) || (time(NULL) < (m_tmLogFileStart + m_i32IntervalInSeconds))))
			{
				return RET_SUC;
			}

			close(false);

			m_tmLogFileStart = time(NULL);

#if __PLATFORM__ == __PLATFORM_LINUX__
			struct tm tmp = { 0 };
			struct tm *pTm = localtime_r(&m_tmLogFileStart, &tmp);
#elif __PLATFORM__ == __PLATFORM_WINDOWS__
			struct tm *pTm = localtime((const time_t*)&m_tmLogFileStart);
#endif

			char szBufA[4096] = { 0 };
#if __PLATFORM__ == __PLATFORM_WINDOWS__
			sprintf(szBufA, "%i\\%2.2i\\%2.2i", pTm->tm_year + 1900, pTm->tm_mon + 1, pTm->tm_mday);
#elif __PLATFORM__ == __PLATFORM_LINUX__
			sprintf(szBufA, "%i/%2.2i/%2.2i", pTm->tm_year + 1900, pTm->tm_mon
					+ 1, pTm->tm_mday);
#endif

			cmn_string_t strLogDir;
			if ("" == m_strLogDir)
			{
				char buf[4096] = { 0 };
				get_cur_exe_path(buf, 4096);
				strLogDir = buf;
				cmn_string_t strExe;
				split_file_name(cmn_string_t(buf), strLogDir, strExe);
			}
			else
			{
				strLogDir = m_strLogDir;
			}
#if __PLATFORM__ == __PLATFORM_WINDOWS__
			cmn_string_t strTemp = strLogDir.substr(0, strLogDir.rfind("\\"));
			strTemp.append("\\logs").append(strLogDir.substr(strLogDir.rfind("\\"))).append("\\").append(szBufA);
#elif __PLATFORM__ == __PLATFORM_LINUX__
			strLogDir.append("/").append(szBufA);
#endif
			int32_t i32Ret = nm_utils::create_recursive_dir_ex(strLogDir.c_str(), 0777);
			if (ZERO > i32Ret)
			{
				CMN_ASSERT(false);
				return -1;
			}

			char szLogFileName[4096] = { 0 };
#if __PLATFORM__ == __PLATFORM_LINUX__
			sprintf(szLogFileName, "%s/%s%2.2i-%2.2i-%2.2i.log",
					strLogDir.c_str(), m_strLogFilePrefix.c_str(), pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
#elif __PLATFORM__ == __PLATFORM_WINDOWS__
			sprintf(szLogFileName, "%s\\%2.2i-%2.2i-%2.2i.log",
					strTemp.c_str(), pTm->tm_hour, pTm->tm_min, pTm->tm_sec);
#endif

			//const string_t strTmp(szLogFileName);
			i32Ret = m_LogFile.create(cmn_string_t(szLogFileName));
			if (ZERO > i32Ret)
			{
				CMN_ASSERT(false);
				return -2;
			}
		}

		return RET_SUC;
	}


#define LOCAL_FMT_BUF_SIZE (4196)
	int CSmartLog::trace_log(cmn_cstr_t pcszFmt, ...)
	{
        va_list ap;
        char szBuf[LOCAL_FMT_BUF_SIZE] = {0};

        va_start(ap, pcszFmt);
        int iRet = vsnprintf(szBuf, LOCAL_FMT_BUF_SIZE, pcszFmt, ap);
        va_end(ap);

        assert(0 < iRet);

//        if (ET_FILE | m_iTraceTarget)
//        {
//        	if (check_log_file() >= 0)
//        	{
//        		fwrite(szBuf, 0 > iRet ? LOCAL_FMT_BUF_SIZE : iRet, 1, m_pLogFile);
//        	}
//        }
//
//        if ((ET_CONSOLE | m_iTraceTarget) && (NULL != stdout))
//        {
//        	fwrite(szBuf, 0 > iRet ? LOCAL_FMT_BUF_SIZE : iRet, 1, stdout);
//        }

		return RET_SUC;
	}

	/**
	 * trace log to target...
	 * */
	void CSmartLog::trace_log(const int32_t i32LogLevel, cmn_cstr_t pcszFileName,
			cmn_cstr_t pcszFunc, int32_t i32LineNo, cmn_cstr_t pcszLog, int32_t i32Target)
	{
		CMN_ASSERT(i32LogLevel >= ELL_DEF && i32LogLevel < ELL_ALL && ZERO != i32Target);
		if (i32LogLevel < m_i32TraceLevel || ZERO == i32Target)
		{
			return;
		}

		cmn_char_t buff[256] = {0};
		cmn_char_t buf[LOG_TEMP_BUF_LEN] = {0};
#if __PLATFORM__ == __PLATFORM_LINUX__
#if defined(__USING_SIMPLE_LOG__)
		int32_t iRet = snprintf(buf, sizeof(buf) / sizeof(cmn_char_t),"%s: %6.6u@%s|%s() => %s" ,
				g_arrLevInfo[i32LogLevel], get_sys_thread_id(), CTimeInfo::get_day_time(buff), pcszFunc, pcszLog);
		CMN_ASSERT(-1 < iRet);
#else
		int iRet = snprintf(buf, sizeof(buf) / sizeof(cmn_char_t), "%s: %6.6u@%s|%s:%4.4i:%s() => %s" ,
			g_arrLevInfo[i32LogLevel], get_sys_thread_id(), CTimeInfo::get_day_time(buff), pcszFileName, i32LineNo, pcszFunc, pcszLog);
		CMN_ASSERT(-1 < iRet);
#endif
#elif __PLATFORM__ == __PLATFORM_WINDOWS__
#if defined(__USING_SIMPLE_LOG__)
		swprintf(buf, sizeof(buf) / sizeof(cmn_char_t),L"%s: %6.6u@%s|%S() => %s" ,
			g_arrLevInfo[i32LogLevel], get_sys_thread_id(), CTimeInfo::get_day_time(buff), pcszFunc, pcszLog);
#else
		swprintf(buf, sizeof(buf) / sizeof(cmn_char_t),L"%s: %6.6u@%s|%s:%4.4i:%S() => %s" ,
				g_arrLevInfo[i32LogLevel], get_sys_thread_id(), CTimeInfo::get_day_time(buff), pcszFileName, i32LineNo, pcszFunc, pcszLog);
#endif
#endif

		if ((ET_FILE & i32Target) && (RET_SUC == check_log_file()))
		{
			m_LogFile.set((cmn_byte_t*)buf, iRet < 0 ? LOG_TEMP_BUF_LEN : iRet);
		}

		if (ET_CONSOLE & i32Target)
		{
			fwrite((cmn_byte_t*)buf, iRet < 0 ? LOG_TEMP_BUF_LEN : iRet, 1, stdout);
		}

//#if defined(__OUTPUT_TO_DEBUG__) && __PLATEFORM__ == __PLATFORM_WINDOWS__
//		::OutputDebugString(buf);
//#endif
	}

	int32_t CSmartLog::get_trace_lev()
	{
		return m_i32TraceLevel;
	}

	int CSmartLog::close(bool bClose)
	{
//		if (NULL != m_pLastLogFile)
//		{
//			fclose(m_pLastLogFile);
//			m_pLastLogFile = NULL;
//		}
		m_LastLogFile.close();

		if (bClose)
		{
			m_LogFile.close();
		}
		else
		{
			m_LastLogFile = m_LogFile;
		}

//		if (m_pLogFile != NULL)
//		{
//			if (bClose)
//			{
//				fclose(m_pLogFile);
//				m_pLogFile = NULL;
//			}
//			else
//			{
//				m_pLastLogFile = m_pLogFile;
//			}
//		}

		return RET_SUC;
	}

}
