/*
 * sn_timer.h
 *
 *  Created on: Feb 23, 2011
 *      Author: rock
 */

#ifndef SN_TIMER_H_
#define SN_TIMER_H_

#include <utils/smart_ptr.h>

#include "../common/sn_common.h"

namespace nm_framework
{

class ITimerObj: public virtual nm_cmn_base::ICommonBase
{
public:
	ITimerObj():m_ui64LastTime(0), m_ui64Interval(0){}
	virtual ~ITimerObj(){}

public:
	virtual void handle_add_into_timer_task() = 0;
	virtual void handle_del_from_timer_task() = 0;
	virtual void set_interval(u_int64_t ui64Interval){m_ui64Interval = ui64Interval;}
	virtual void on_timer() = 0;
	void check(u_int64_t ui64CurTimeInUs)
	{
		if (0 == m_ui64LastTime)
		{
			m_ui64LastTime = ui64CurTimeInUs;
		}
		else if (ui64CurTimeInUs >= (m_ui64LastTime + m_ui64Interval))
		{
			m_ui64LastTime = ui64CurTimeInUs;
		}
		else
		{
			return;
		}

		on_timer();
	}

private:
	u_int64_t m_ui64LastTime;
	u_int64_t m_ui64Interval;
};
typedef nm_utils::CSmartPtr<ITimerObj> timer_obj_ptr_t;

}

#endif /* SN_TIMER_H_ */
