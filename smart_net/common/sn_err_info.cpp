/*
 * sn_err_info.cpp
 *
 *  Created on: Feb 12, 2011
 *      Author: rock
 */

#include "sn_err_info.h"
#include <common/common.h>

const char* arrSnErrInfo[] =
{
		"create socket failed!",
		"The socket is nonblocking and the connection cannot be completed immediately"
};

const char* get_sn_err_info(int32_t i32ErrCode)
{
	SYS_ASSERT(SNERR_DEF < i32ErrCode);
	return arrSnErrInfo[i32ErrCode - SNERR_DEF - 1];
}
