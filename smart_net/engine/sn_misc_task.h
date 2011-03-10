/*
 * sn_misc_task.h
 *
 *  Created on: Mar 8, 2011
 *      Author: rock
 */

#ifndef __SN_MISC_TASK_H__
#define __SN_MISC_TASK_H__

#include <thread/thread_ex.h>

#include "sn_io_obj.h"
#include "sn_timer.h"
#include "sn_io_evt_notifier.h"

namespace nm_engine
{

class CMiscTask : public nm_thread::IThreadTask
{
public:
	CMiscTask();
	virtual ~CMiscTask();

public:
	///
	virtual int32_t add_io_obj(const io_obj_ptr_t &pioobj);
	virtual int32_t del_io_obj(const io_obj_ptr_t &pioobj);
	///
	virtual int32_t add_timer(const timer_ptr_t &ptimer);
	virtual int32_t del_timer(const timer_ptr_t &ptimer);

private:
	io_evt_notifier_ptr_t m_pioevtnotifier;

};
typedef nm_utils::CSmartPtr<CMiscTask> misc_task_ptr_t;

}

#endif /* __SN_MISC_TASK_H__ */
