/*
 * err_info.h
 *
 *  Created on: Feb 12, 2011
 *      Author: rock
 */

#ifndef __ERR_INFO_H__
#define __ERR_INFO_H__

#include "types.h"
#include <errno.h>

#define COMMON_ERR_CODE_BEGINNING (-10000) //-10000 ~ -1

enum
{
	CMNERR_SUC = 0, ///no error.
	CMNERR_BEGINNING = COMMON_ERR_CODE_BEGINNING,
	CMNERR_INVALID_PARAMS, ///invalid parameters.
	CMNERR_NO_MATCH_ITEM, ///no matched item.
	CMNERR_CALL_SYS_FUN_FAILED,
	CMNERR_COMMON_ERR,
	CMNERR_FATAL_ERR,
	CMNERR_IO_ERR,
	CMNERR_SEND_PENDING,
	CMNERR_RECV_PENDING,
	CMNERR_INCOMPLETED_PKG,
	CMNERR_UNKNOWN_PKG
};

const char* get_cmn_err_info(int32_t i32ErrCode);


#endif /* ERR_INFO_H_ */
