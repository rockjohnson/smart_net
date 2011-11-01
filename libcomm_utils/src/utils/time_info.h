/*
 * time_info.h
 *
 *  Created on: Apr 3, 2009
 *      Author: rock
 */

#ifndef __TIME_INFO_H__
#define __TIME_INFO_H__

#include "../common/common.h"

namespace nm_utils
{
	class CTimeInfo
	{
	public:
		CTimeInfo();
		virtual ~CTimeInfo();

	public:
		static time_t get_time_in_seconds();
		static cmn_cstr_t get_day_time(cmn_char_t *pwszBuf);
		static u_int64_t get_current_time_us();
	};
}

#endif /* __TIME_INFO_H__ */
