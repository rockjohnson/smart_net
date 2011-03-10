/*
 * sn_time_notifier.h
 *
 *  Created on: Mar 10, 2011
 *      Author: rock
 */

#ifndef __SN_TIME_NOTIFIER_H__
#define __SN_TIME_NOTIFIER_H__

#include <set>

#include "../common/sn_common.h"
#include "sn_timer.h"

namespace nm_engine
{

/**
 * not thread safe
 * */
class CTimeNotifier : public nm_base::ICommonBase
{
public:
	CTimeNotifier();
	virtual ~CTimeNotifier();

public:
    int32_t add_timer(const timer_ptr_t &ptimer);
    int32_t del_timer(const timer_ptr_t &ptimer);

    void check();
private:
	typedef std::set<timer_ptr_t> timer_set_t;
	typedef std::pair<timer_set_t::iterator, bool> timer_set_ret_t;
	timer_set_t m_settimers;
};

}

#endif /* __SN_TIME_NOTIFIER_H__ */
