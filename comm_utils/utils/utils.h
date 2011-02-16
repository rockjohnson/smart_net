/*
 * comm.h
 *
 *  Created on: Apr 8, 2009
 *      Author: rock
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include "../common/common.h"

#if __PLATFORM__ == __PLATFORM_LINUX__
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <pthread.h>
#include <vector>
#elif defined(__PLATEFORM_WINDOWS__)
#include <Windows.h>
#endif

#include <sys/types.h>

#include "../common/common.h"


namespace nm_utils
{

	inline u_int32_t get_sys_thread_id() //get the thread id of the calling thread.
	{
#if __PLATFORM__ == __PLATFORM_LINUX__
		return ::syscall(SYS_gettid);
#elif __PLATFORM__ == __PLATFORM_WINDOWS__
		return ::GetCurrentThreadId();
#endif
	}

#if __PLATFORM__ == __PLATFORM_LINUX__
	inline int get_max_fd_no(int *pIsoft, int *pIhard)
	{
		struct rlimit rl = {0};

		int iRet = getrlimit(RLIMIT_NOFILE, &rl);

		*pIsoft = rl.rlim_cur;
		*pIhard = rl.rlim_max;

		return iRet;
	}

	//an  unprivileged process may only set its soft limit to a value in the range from 0 up to the hard limit, and (irre-
    //versibly) lower its hard limit.  A privileged process (under Linux: one with the CAP_SYS_RESOURCE capability) may  make  arbitrary
    //changes to either limit value.
	//so when you set the limit,pleas note the return value.
	inline int set_max_fd_no(int iSoft, int iHard)
	{
		struct rlimit rl = {iSoft, iHard};

		return setrlimit(RLIMIT_NOFILE, &rl);
	}

	inline int set_max_core_size(int iSoft, int iHard)
	{
		struct rlimit rl = {iSoft, iHard};

		return setrlimit(RLIMIT_CORE, &rl);
	}

	int set_max_res_lim_size(int iRes, u_long ulSoft, u_long ulHard);
	int get_max_res_lim_size(int iRes, u_long &iSoft, u_long &iHard);

	//return errno. 0 : success;
	int get_thread_sched_attr(pthread_t th_id, int &iPolicy, int &iPriority);
	int set_thread_sched_attr(pthread_t th_id, int iPolicy, int iPriority);
	//
	int get_thread_nice_val(int iThId, int &iNiceVal);
	int set_thread_nice_val(int iThId, int iNiceVal);

	//signal
	void* set_signal_handler(int iSig, void (*pFun)(int));

	//affinity
	int set_thread_affinity(pthread_t th_id, int iCPUNum);
	int get_thread_affinity(pthread_t th_id, std::vector<int> &vecCPUs);
#endif
	
	//emulate sleep
	int sleep(long int lSec, long int lMicroSec);
	//
	bool is_little_endian();

	//get executable path
	char* get_cur_exe_path(char *pBytesBuf, u_int32_t uiBufSize);

	//if file or directory exist?
	int if_file_or_dir_exist(const char *pszName);

	//create recursive directory
	int create_recursive_dir(const char *pszName, u_int32_t md);
	int create_recursive_dir_ex(const char *pszName, u_int32_t md); //better

	//get cpu count
	inline u_int32_t get_cpu_cnt()
	{
		return sysconf(_SC_NPROCESSORS_ONLN);
	}

	///split file name
	int32_t split_file_name(const string_t &strFullFileName, string_t &strDir, string_t &strFile);

	///get sys error msg
	const char* get_sys_err_msg(int32_t iErrNo, char *pszErrBuf, u_int32_t uiBufSize);

	///get or set socket block.
	int32_t get_block_flag(int32_t i32fd);
	int32_t set_block_flag(int32_t i32fd, bool bBlockOrNot);

	///convert ip
	int32_t ip_ston(int32_t i32af, cstr_t pcszSrcIp, pvoid_t pDst);
	/*
	 * e.g.
	 * struct in_addr addr;
	 * ip_ston(AF_INET, "127.0.0.1", (pvoid_t)&addr);
	 * struct in6_addr addr;
	 * ip_ston(AF_INET6, "127.0.0.1", (pvoid_t)&addr);
	 * */
	int32_t ip_ntos(int32_t i32af, pvoid_t pSrcIp, char_t *pszDst, const socklen_t cslDstLen);
	/*
	 * e.g.
	 * struct in_addr addr;
	 * ...
	 * char_t buf[INET_ADDRSTRLEN] = {0};
	 * ip_ntos(AF_INET, (pvoid_t)&addr, buf, INET_ADDRSTRLEN);
	 *
	 * struct in6_addr addr;
	 * ...
	 *
	 * */
}
#endif /* __COMM_H__ */
