/*
 * comm.cpp
 *
 *  Created on: Apr 23, 2009
 *      Author: rock
 */

#include "utils.h"

#include <stdio.h>
#include <signal.h>
#if __PLATFORM__ == __PLATFORM_LINUX__
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#endif
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <arpa/inet.h>

#if __PLATFORM__ == __PLATFORM_WINDOWS__
#include <Winsock2.h>
#include <shlobj.h>
#include <shlwapi.h>
#elif __PLATFORM__ == __PLATFORM_LINUX__
#endif

//attention!!!! when i using "using namespace nm_utils" in this context, i get a undefined get_thread_sched_attr
//error when make a program linking, but using "namespace nm_utils { }" is ok.
//using namespace nm_utils;
namespace nm_utils
{
#if __PLATFORM__ == __PLATFORM_LINUX__

	int set_max_res_lim_size(int iRes, u_long ulSoft, u_long ulHard)
	{
		struct rlimit rl =
		{ ulSoft, ulHard };
		errno = 0;
		setrlimit(iRes, &rl);
		return errno;
	}

	int get_max_res_lim_size(int iRes, u_long & ulSoft, u_long & ulHard)
	{
		struct rlimit rl =
		{ 0 };
		errno = 0;
		if (0 == getrlimit(iRes, &rl))
		{
			ulSoft = rl.rlim_cur;
			ulHard = rl.rlim_max;
		}
		return errno;
	}

	int get_thread_sched_attr(pthread_t th_id, int & iPolicy, int & iPriority)
	{
		//int policy;
		//struct sched_param param;
		return pthread_getschedparam(/*pthread_self()*/th_id, &iPolicy,
				(struct sched_param*) &iPriority);
	}

	int set_thread_sched_attr(pthread_t th_id, int iPolicy, int iPriority)
	{
		return ::pthread_setschedparam(th_id, iPolicy, (const sched_param*)&iPriority);
	}

	int get_thread_nice_val(int iThId, int & iNiceVal)
	{
		errno = 0;
		iNiceVal = ::getpriority(PRIO_PROCESS, iThId);
		return errno;
	}

	int set_thread_nice_val(int iThId, int iNiceVal)
	{
		errno = 0;
		::setpriority(PRIO_PROCESS, iThId, iNiceVal);
		return errno;
	}

	/* Signale wrapper. */
	void *set_signal_handler(int signo, void(*func)(int))
	{
		struct sigaction sig;
		struct sigaction osig;
		sig.sa_handler = func;
		sigemptyset(&sig.sa_mask); // sa_mask specifies a mask of signals which should be blocked (i.e., added to the signal mask of the thread in which the  signal  handler
		//is  invoked)  during  execution of the signal handler.  In addition, the signal which triggered the handler will be blocked, unless the
		//SA_NODEFER flag is used.
		sig.sa_flags = 0;
#ifdef SA_RESTART
		sig.sa_flags |= SA_RESTART;
#endif /* SA_RESTART */
		int ret = sigaction(signo, &sig, &osig);
		if (ret < 0)
			return NULL;
		else
			return (void*) (osig.sa_handler);
	}

	int set_thread_affinity(pthread_t th_id, int iCPUNum)
	{
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		CPU_SET(iCPUNum, &cpuset);

		return pthread_setaffinity_np(th_id, sizeof(cpu_set_t), &cpuset);
	}

	int get_thread_affinity(pthread_t th_id, std::vector<int> &vecCPUs)
	{
		cpu_set_t cpuset;
		CPU_ZERO(&cpuset);
		/* Check the actual affinity mask assigned to the thread */
		int iRet = pthread_getaffinity_np(th_id, sizeof(cpu_set_t), &cpuset);
		if (0 != iRet)
		{
			return iRet;
		}

		for (int i = 0; i < CPU_SETSIZE; i++)
		{
			if (CPU_ISSET(i, &cpuset))
			{
				vecCPUs.push_back(i);
			}
		}

		return iRet;
	}

#endif

	int sleep(long int lSec, long int lMicroSec/*1sec = 1000 millisecond = 1000000 microsec*/)
	{

#if __PLATFORM__ == __PLATFORM_WINDOWS__
		Sleep(lSec*1000 + lMicroSec/1000);
		return 0;
#else
		struct timeval tv =
		{ 0 };
		tv.tv_sec = lSec;
		tv.tv_usec = lMicroSec;
		return select(0, NULL, NULL, NULL, &tv);
#endif
	}

	bool is_little_endian()
	{
		u_int16_t usWord = 0x1234;
		return 0x34 == *((char*)&usWord) && 0x12 == *(((char*)&usWord)+1);
	}

	char *get_cur_exe_path(char *pBytesBuf, u_int32_t uiBufSize)
	{
#if __PLATFORM__ == __PLATFORM_WINDOWS__
		DWORD ret = GetModuleFileNameA(NULL, pBytesBuf, uiBufSize);
		if (0 == ret)
		{
			return NULL;
		}

#elif __PLATFORM__ == __PLATFORM_LINUX__
		char linkname[64] =
		{ 0 }; /* /proc/<pid>/exe */
		/* Get our PID and build the name of the link in /proc */
		if (snprintf(linkname, sizeof(linkname), "/proc/%i/exe", getpid()) < 0)
		{
			/* This should only happen on large word systems. I'm not sure
			 what the proper response is here.
			 Since it really is an assert-like condition, aborting the
			 program seems to be in order. */
			//abort();
			return NULL;
		}
		/* Now read the symbolic link */
		int ret = readlink(linkname, pBytesBuf, uiBufSize);

		/* In case of an error, leave the handling up to the caller */
		if (ret == -1)
			return NULL;
#endif
		/* Report insufficient buffer size */
		if (ret >= uiBufSize)
		{
			//errno = ERANGE;
			return NULL;
		}
		/* Ensure proper NUL termination */
		pBytesBuf[ret] = 0;
		return pBytesBuf;

	}

	int if_file_or_dir_exist(const char *pcszName)
	{
		SYS_ASSERT(NULL != pcszName);
#if __PLATFORM__ == __PLATFORM_WINDOWS__
		return ::PathFileExistsA(pcszName) ? 0 : -1;
#elif __PLATFORM__ == __PLATFORM_LINUX__
		return access(pcszName, F_OK);
#endif
	}

#define RETURN_IF_FAIL(FUN, RET) \
	if ((FUN) < 0) \
	{\
		SYS_ASSERT(0 == chdir(szCwd)); \
		return (RET); \
	}
#define RETURN_IF_FAIL_EX(FUN, RET) \
	if ((FUN) < 0) \
	{\
		return (RET); \
	}
#define DELIMITERS "/"

#if __PLATFORM__ == __PLATFORM_LINUX__
	int create_recursive_dir(const char *pszName, u_int32_t md)
	{
		//split string
		SYS_ASSERT(NULL != pszName);
		//record current working directory
		char szCwd[1024] =
		{ 0 };
		if (NULL == getcwd(szCwd, 1024))
		{
			return -10;
		}
		char *pszTemp = strdupa(pszName);
		char *pszToken = strtok(pszTemp, DELIMITERS);
		if (NULL == pszToken)
		{
			return -1;
		}
		//int iRet = 0;
		//if absolutely path
		if ('/' == *pszName)
		{
			//absolutely path
			char szTemp[1024] = DELIMITERS;
			strcat(szTemp, pszToken);
			if (if_file_or_dir_exist(szTemp) < 0)
			{
				RETURN_IF_FAIL(mkdir(szTemp, md), -2)
			}
			RETURN_IF_FAIL(chdir(szTemp), -3)
		}
		else
		{
			if (if_file_or_dir_exist(pszToken) < 0)
			{
				RETURN_IF_FAIL(mkdir(pszToken, md), -4)
			}
			RETURN_IF_FAIL(chdir(pszToken), -5)
		}
		while (NULL != (pszToken = strtok(NULL, DELIMITERS)))
		{
			if (if_file_or_dir_exist(pszToken) < 0)
			{
				RETURN_IF_FAIL(mkdir(pszToken, md), -6)
			}
			RETURN_IF_FAIL(chdir(pszToken), -7)
		}
		//reset current working directory
		SYS_ASSERT(0 == chdir(szCwd));
		return 0;
	}
#endif

	int create_recursive_dir_ex(const char *pcszDir, u_int32_t md)
	{
		SYS_ASSERT(NULL != pcszDir);

#if __PLATFORM__ == __PLATEFORM_WINDOWS__
		if (ERROR_SUCCESS != ::SHCreateDirectoryExA(0, pcszDir, 0))
		{
			if(ERROR_ALREADY_EXISTS != ::GetLastError())
			{
				return -1;
			}
		}
#elif __PLATFORM__ == __PLATFORM_LINUX__
		char *pszTemp = strdupa(pcszDir);
		char *pszToken = strtok(pszTemp, DELIMITERS);
		if (NULL == pszToken)
		{
			return -1;
		}

		char szTemp[4096] = { 0 };
		//if absolutely path
		if ('/' == *pcszDir)
		{
			//absolutely path
			szTemp[0] = '/';
			strcat(szTemp, pszToken);
		}
		else
		{
			strcpy(szTemp, pszToken);
		}
		if (if_file_or_dir_exist(szTemp) < 0)
		{
			RETURN_IF_FAIL_EX(mkdir(szTemp, md), -2)
		}

		while (NULL != (pszToken = strtok(NULL, DELIMITERS)))
		{
			strcat(szTemp, DELIMITERS);
			strcat(szTemp, pszToken);
			if (if_file_or_dir_exist(szTemp) < 0)
			{
				RETURN_IF_FAIL_EX(mkdir(szTemp, md), -3)
			}
		}
#endif

		return 0;
	}

	int32_t split_file_name(const cmn_string_t &strFullName, cmn_string_t &strDir, cmn_string_t &strFile)
	{
	  size_t sztFound  = strFullName.find_last_of("/\\");
	  if (cmn_string_t::npos == sztFound)
	  {
		  return RET_ERR;
	  }

	  strDir = strFullName.substr(0, sztFound);
	  strFile = strFullName.substr(sztFound + 1, cmn_string_t::npos);

	  return RET_SUC;
	}

//static void display_sched_attr(int policy, struct sched_param *param)
//{
//	printf("    policy=%s, priority=%d\n",
//			(policy == SCHED_FIFO) ? "SCHED_FIFO"
//					: (policy == SCHED_RR) ? "SCHED_RR" : (policy
//							== SCHED_OTHER) ? "SCHED_OTHER" : "???",
//									param->sched_priority);
//} /* display_sched_attr */

    const char* get_sys_err_msg(int32_t iErrNo, char *pszErrBuf, u_int32_t uiBufSize)
    {
#if __PLATFORM__ == __PLATFORM_LINUX__
            char *psz = NULL;
            return (psz = (char*)strerror_r(iErrNo, pszErrBuf, uiBufSize)) == NULL ? pszErrBuf
                            : strcpy(pszErrBuf, psz); //note, according to the definition of the strerror_r, pszErrBuf may not be filled with the error message, but the return does.
#elif defined(__PLATEFORM_WINDOWS__)
            void *lpMsgBuf = 0;

            ::FormatMessageA(
                    FORMAT_MESSAGE_ALLOCATE_BUFFER |
                    FORMAT_MESSAGE_FROM_SYSTEM,
                    NULL,
                    iErrNo,
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                    (char*)&lpMsgBuf,
                    0, NULL );
            strcpy_s(pszErrBuf, uiBufSize, (const char*)lpMsgBuf);
            ::LocalFree(lpMsgBuf);

            return pszErrBuf;
#endif
    }


    int32_t get_block_flag(int32_t i32fd)
    {
    	SYS_ASSERT(-1 < i32fd);
    	return fcntl(i32fd, F_GETFL, 0);
    }

    int32_t set_block_flag(int32_t i32fd, bool bBlockOrNot)
    {
    	SYS_ASSERT(-1 < i32fd);
    	int32_t i32Flags = get_block_flag(i32fd);
    	if (0 > i32Flags)
    	{
    		return CMNERR_CALL_SYS_FUN_FAILED;
    	}

    	return bBlockOrNot ?
    			fcntl(i32fd, F_SETFL, i32Flags & (~O_NONBLOCK)) :
    			fcntl(i32fd, F_SETFL, i32Flags | O_NONBLOCK);
    }

    ///
int32_t ip_ston(int32_t i32af, cmn_cstr_t pcszIp, cmn_pvoid_t pDst)
{
	if (AF_INET != i32af && AF_INET6 != i32af)
	{
		return CMNERR_COMMON_ERR;
	}

	return inet_pton(i32af, pcszIp, pDst) <= 0 ? CMNERR_COMMON_ERR : CMNERR_SUC;
}

int32_t ip_ntos(int32_t i32af, cmn_pvoid_t pSrcIp, cmn_char_t *pszDst, const socklen_t cslDstLen)
{
	if (AF_INET != i32af && AF_INET6 != i32af)
	{
		return CMNERR_COMMON_ERR;
	}

	if (INET_ADDRSTRLEN > cslDstLen || (AF_INET6 == i32af && INET6_ADDRSTRLEN > cslDstLen))
	{
		return CMNERR_COMMON_ERR;
	}

	if (inet_ntop(i32af, pSrcIp, pszDst, cslDstLen) == NULL)
	{
		return CMNERR_COMMON_ERR;
	}

	return CMNERR_SUC;
}

}

