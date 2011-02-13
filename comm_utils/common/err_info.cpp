/*
 * err_info.cpp
 *
 *  Created on: Feb 12, 2011
 *      Author: rock
 */

#include "err_info.h"
#include "defines.h"

static const char* arrCmnErrInfo[] =
{
		"no error, successful"
		"invalid parameters",
		"no matched iterm",
		"call system function failed"
};

const char* get_cmn_err_info(int32_t i32ErrCode)
{
	SYS_ASSERT(CMNERR_BEGINNING < i32ErrCode || CMNERR_SUC == i32ErrCode);
	return arrCmnErrInfo[CMNERR_SUC == i32ErrCode ? i32ErrCode : i32ErrCode - CMNERR_BEGINNING - 1];
}
