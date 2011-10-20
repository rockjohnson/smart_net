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

	int32_t CTimeNotifier::add_timer(const timer_obj_ptr_t &pTimer)
	{
		nm_utils::spin_scopelk_t lk(m_lkTimerAddCache);

		m_vecTimerAddCache.push_back(pTimer);

		return CMNERR_SUC;
	}

	int32_t CTimeNotifier::del_timer(const timer_obj_ptr_t &pTimer)
	{
		nm_utils::spin_scopelk_t lk(m_lkTimerDelCache);

		m_vecTimerDelCache.push_back(pTimer);

		return CMNERR_SUC;
	}

#define MORE (10)
	void CTimeNotifier::exec()
	{
		///delete first
		if (!m_vecTimerDelCache.empty())
		{
			timer_vec_t vecTmp(m_vecTimerDelCache.size() + MORE);
			{
				nm_utils::spin_scopelk_t lk(m_lkTimerDelCache);
				vecTmp.swap(m_vecTimerDelCache);
				CMN_ASSERT(m_vecTimerDelCache.empty());
			}

			///delete cache
			for (timer_vec_t::iterator iter = m_vecTimerDelCache.begin(); iter != m_vecTimerDelCache.end(); ++iter)
			{
				timer_set_t::size_type ret = m_setTimers.erase(*iter);
				CMN_ASSERT(1 == ret);
				(*iter)->handle_del_from_timer_task();
			}
		}

		///add
		if (!m_vecTimerAddCache.empty())
		{
			timer_vec_t vecTmp(m_vecTimerAddCache.size() + MORE);

			{
				nm_utils::spin_scopelk_t lk(m_lkTimerAddCache);
				vecTmp.swap(m_vecTimerAddCache);
				CMN_ASSERT(m_vecTimerAddCache.empty());
			}

			///add cache.
			for (timer_vec_t::iterator iter = m_vecTimerAddCache.begin(); iter != m_vecTimerAddCache.end(); iter++)
			{
				timer_set_ret_t ret = m_setTimers.insert(*iter);
				CMN_ASSERT(ret.second);
				(*iter)->handle_add_into_timer_task();
			}
		}

		///
		u_int64_t ui64CurTime = nm_utils::CTimeInfo::get_current_time_us();
		for (timer_set_t::iterator iter = m_setTimers.begin(); iter != m_setTimers.end(); ++iter)
		{
			(*iter)->check(ui64CurTime);
		}
	}

}
