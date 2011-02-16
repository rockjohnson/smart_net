/*
 * log.h
 *
 *  Created on: Apr 3, 2009
 *      Author: rock
 */

#ifndef __SMART_LOG_H__
#define __SMART_LOG_H__

#if __PLATFORM__ == __PLATEFORM_LINUX__
#include <unistd.h>
#endif
#include <sys/types.h>
#include <assert.h>
#include <stdio.h>
#include <stddef.h>
#include <string>

#include "../common/common.h"
#include "../utils/time_info.h"
#include "../utils/smart_lock.h"
#include "../utils/file.h"

namespace nm_utils
{
	enum ELogLevel
	{
		ELL_DEF = 0,
		ELL_DEBUG,
		ELL_INFO,
		ELL_WARNING,
		ELL_ERR,
		ELL_ALL
	};

	enum ELogTarget
	{
		ET_FILE = 0x01,
		ET_CONSOLE = 0x02
//		ET_DB = 0x04
	};

	/**
	 * smart log class
	 * */
	class CSmartLog : public nm_base::CNoncopyable
	{
	public:
		CSmartLog();
		virtual ~CSmartLog();

	public:
		///you should call this func after you construct a smart log obj firtly!
		int32_t init(cstr_t pcszLogDir, cstr_t pcszLogFilePrefix, int32_t i32LogLevel, int32_t i32Interval);
		///the trace log func
		void trace_log(const int32_t iLogLev, cstr_t pcszFileName, cstr_t pcszFunc, int32_t i32LineNo, cstr_t pcszLog, int32_t i32Target);
		///...
		int32_t trace_log(cstr_t pcszFmt, ...);
		///you should close the log after your using.
		int32_t close(bool bClose = false);
		///
		int32_t get_trace_lev();

	private:
		//int init_log(cstr_t strFileLog);
		int check_log_file();

	private:
		CLiteFile m_LogFile;
		CLiteFile m_LastLogFile; //for avoding multithreading race condition.
		int32_t m_i32TraceLevel; //the log level.
		string_t m_strLogDir;
		string_t m_strLogFilePrefix;

#if __PLATFORM__ == __PLATFORM_LINUX__
		time_t m_tmLogFileStart; //when the log file is started
#elif __PLATFORM__ == __PLATFORM_WINDOWS__
		time_t m_tmLogFileStart;
#endif
		u_int32_t m_i32Interval; //the log file interval
		CMutexLock m_lock;
	};


#if __PLATFORM__ == __PLATFORM_LINUX__
#define TRACE_EX(log_obj, log_level, contents) \
        log_obj.trace_log(log_level, __FILE__, __func__, __LINE__, contents)

#define LOCAL_LOG_TMP_BUF_SZ (8192)
#define TRACE_LOG(log_obj, log_level, ...) \
		{ \
	if (log_level >= log_obj.get_trace_lev()) \
	{ \
		char_t __tmp_buf__[LOCAL_LOG_TMP_BUF_SZ] = {0}; \
		snprintf(__tmp_buf__, LOCAL_LOG_TMP_BUF_SZ, __VA_ARGS__); \
		log_obj.trace_log(log_level, __FILE__, __func__, __LINE__, __tmp_buf__, nm_utils::ET_FILE | nm_utils::ET_CONSOLE); \
		} \
		}

#define TRACE_LAST_ERR(log_obj, expr) \
		{ \
			int32_t iErrCode = errno; \
			char_t szBuf[1024] = {0}; \
			TRACE_LOG(log_obj, ELL_ERR, "%s: %s(%d)\n", #expr, get_sys_err_msg(iErrCode, szBuf, sizeof(szBuf)), iErrCode); \
		}


#elif __PLATFORM__ == __PLATFORM_WINDOWS__
#define TRACE_EX(log_file, log_level, contents) \
        log_file.trace_log(log_level, ANSI_TO_UCS4(__FILE__), __FUNCTION__, __LINE__, contents)
#endif

#if defined(__DEBUG_VER__)
#define LOG_ASSERT(expr) assert((expr))
#else
#define LOG_ASSERT(expr) \
	if (!(expr)) \
	{ \
		TRACE_LOG(ELL_ASSERT, "assert failed: %s\n", #expr);
	}
#endif

}

#endif /* __SMART_LOG_H__ */
