/*
 * sn_misc_task.h
 *
 *  Created on: Mar 8, 2011
 *      Author: rock
 */

#ifndef __SN_MISC_TASK_H__
#define __SN_MISC_TASK_H__

#include <thread/thread_ex.h>
#include <utils/event_engine.h>

#include "sn_io_obj.h"
#include "sn_timer.h"
#include "sn_io_evt_notifier.h"
#include "sn_time_notifier.h"

namespace nm_engine
{

/**
 * not thread safe
 * */
class CMiscTask : public nm_thread::IThreadTask
{
public:
	CMiscTask();
	virtual ~CMiscTask();

public:
	///
	int32_t init(int32_t i32ioevtnotifier, int32_t i32MStimeout);
	int32_t destroy();
	///
	int32_t add_io_obj(const io_obj_ptr_t &pioobj);
	int32_t del_io_obj(const io_obj_ptr_t &pioobj);
	///
	int32_t add_timer(const timer_ptr_t &ptimer);
	int32_t del_timer(const timer_ptr_t &ptimer);
	///
	void post_evt(nm_utils::event_ptr_t &pevt);
	///
	virtual void exec();

private:
	///
	ioevt_notifier_ptr_t m_pioevtnotifier;
	///
	time_notifier_t m_ptimenotifier;
	///
	nm_utils::CEventHandleEngine m_evtengine;
};
typedef nm_utils::CSmartPtr<CMiscTask> misc_task_ptr_t;

}

#endif /* __SN_MISC_TASK_H__ */
