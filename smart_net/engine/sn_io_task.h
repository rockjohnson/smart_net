/*
 * io_task.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef IO_TASK_H_
#define IO_TASK_H_

#include <thread/thread_ex.h>
#include "sn_io_evt_notify.h"
#include <utils/smart_lock.h>
#include "sn_timer.h"

namespace nm_engine
{

/**
 *
 * */
class CIoTask: public nm_thread::IThreadTask
{
public:
	CIoTask();
	virtual ~CIoTask();

public:
	///
	int32_t init(int32_t i32IoEvtNotifier, int32_t i32MsTimeout, int32_t i32ID);
	int32_t destroy();
	///
	void exec();
	///
	int32_t add_io_obj(const io_obj_ptr_t &pIoObj);
	int32_t del_io_obj(const io_obj_ptr_t &pIoObj);
	///
	int32_t add_timer(timer_ptr_t &pInternalTimer);
	int32_t del_timer(timer_ptr_t &pInternalTimer);

private:
	void update_io_set();
	void handle_io_evts();

private:
	io_evt_notify_ptr_t m_pIoEvtNotifier; ///the io event notify mechanism obj;

	typedef std::set<nm_framework::io_obj_ptr_t> io_obj_set_t;
	typedef std::pair<io_obj_set_t::iterator, bool> io_obj_set_ret_t;
	///io obj add cache
	io_obj_set_t m_setIoObjAddCache;
	io_obj_set_t m_setIoObjDelCache;
	CSpinLock m_lkIoObjCache;
	///the io objs
	io_obj_set_t m_setIoObjs;
};
typedef nm_utils::CSmartPtr<nm_framework::CIoTask> io_task_ptr_t;

}

#endif /* IO_TASK_H_ */
