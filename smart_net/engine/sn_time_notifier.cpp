/*
 * sn_time_notifier.cpp
 *
 *  Created on: Mar 10, 2011
 *      Author: rock
 */

#include "sn_time_notifier.h"

#include <utils/time_info.h>

namespace nm_engine
{

CTimeNotifier::CTimeNotifier()
{
	// TODO Auto-generated constructor stub
}

CTimeNotifier::~CTimeNotifier()
{
	// TODO Auto-generated destructor stub
}

int32_t CTimeNotifier::add_timer(const timer_ptr_t &ptimer)
{
	timer_set_ret_t ret = m_settimers.insert(ptimer);

	return ret.second ? CMNERR_SUC : CMNERR_COMMON_ERR;
}

int32_t CTimeNotifier::del_timer(const timer_ptr_t &ptimer)
{
	timer_set_ret_t ret = m_settimers.erase(ptimer);

	return ret.second ? CMNERR_SUC : CMNERR_COMMON_ERR;
}

void CTimeNotifier::check()
{
	u_int64_t ui64curtime = nm_utils::CTimeInfo::get_current_time_us();

	for (timer_set_t::iterator iter = m_settimers.begin(); iter != m_settimers.end(); iter++)
	{
		(*iter)->check(ui64curtime);
	}
}

}
