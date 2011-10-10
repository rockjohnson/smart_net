/*
 * sn_time_notifier.cpp
 *
 *  Created on: Mar 10, 2011
 *      Author: rock
 */

#include "sn_time_notifier.h"

#include <utils/time_info.h>

namespace nm_framework
{

CTimeNotifier::CTimeNotifier()
{
	// TODO Auto-generated constructor stub
}

CTimeNotifier::~CTimeNotifier()
{
	// TODO Auto-generated destructor stub
}

int32_t CTimeNotifier::add_timer(const timer_obj_ptr_t &ptimer)
{
	nm_utils::spin_scopelk_t lk(m_lktimercache);

	///delete cache
	timer_set_t::iterator iter = m_settimerdelcache.find(ptimer);
	IF_TRUE_THEN_RETURN_CODE(iter != m_settimerdelcache.end(), CMNERR_COMMON_ERR);

	///timer set
	iter = m_settimers.find(ptimer);
	IF_TRUE_THEN_RETURN_CODE(iter != m_settimers.end(), CMNERR_COMMON_ERR);

	timer_set_ret_t ret = m_settimeraddcache.insert(ptimer);

	return ret.second ? CMNERR_SUC : CMNERR_COMMON_ERR;
}

int32_t CTimeNotifier::del_timer(const timer_obj_ptr_t &ptimer)
{
	nm_utils::spin_scopelk_t lk(m_lktimercache);

	///add cache
	timer_set_t::iterator iter = m_settimeraddcache.find(ptimer);
	IF_TRUE_THEN_RETURN_CODE(iter != m_settimeraddcache.end(), CMNERR_COMMON_ERR);

	///timer set
	iter = m_settimers.find(ptimer);
	IF_TRUE_THEN_RETURN_CODE(iter == m_settimers.end(), CMNERR_COMMON_ERR);

	timer_set_ret_t ret = m_settimerdelcache.insert(ptimer);

	return ret.second ? CMNERR_SUC : CMNERR_COMMON_ERR;
}

void CTimeNotifier::exec()
{
	///update set
	if (!m_settimeraddcache.empty() || !m_settimerdelcache.empty())
	{
		nm_utils::spin_scopelk_t lk(m_lktimercache);

		///delete cache
		for (timer_set_t::iterator iter = m_settimerdelcache.begin(); iter != m_settimerdelcache.end(); iter++)
		{
			m_settimers.erase(*iter);
		}
		m_settimerdelcache.clear();

		///add cache.
		for (timer_set_t::iterator iter = m_settimeraddcache.begin(); iter != m_settimeraddcache.end(); iter++)
		{
			m_settimers.insert(*iter);
		}
		m_settimeraddcache.clear();
	}

	///
	u_int64_t ui64curtime = nm_utils::CTimeInfo::get_current_time_us();
	for (timer_set_t::iterator iter = m_settimers.begin(); iter != m_settimers.end(); iter++)
	{
		(*iter)->check(ui64curtime);
	}
}

}
