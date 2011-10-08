/*
 * io_task.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef IO_TASK_H_
#define IO_TASK_H_

#include <thread/thread_ex.h>
#include <utils/smart_lock.h>
#include <utils/event_engine.h>

#include "sn_io_evt_notifier.h"
#include "sn_timer.h"

namespace nm_engine
{

	enum E_IO_TASK_TYPE
	{
		EITT_HANDLE_INPUT_TASK = 0,
		EITT_HANDLE_OUTPUT_TASK
	};

/**
 * input handler
 * */
class CInputHandleTask: public nm_thread::IThreadTask
{
public:
	CInputHandleTask();
	virtual ~CInputHandleTask();

public:
	///
	int32_t init(int32_t i32ioevtnotifier, int32_t i32MStimeout, int32_t i32id);
	int32_t destroy();
	///
	void exec();
	///
	void post_event(const nm_utils::event_ptr_t &pevt);
	int32_t add_io_obj(const io_obj_ptr_t &pioobj);
	int32_t del_io_obj(const io_obj_ptr_t &pioobj);
	///
	int32_t get_ioobj_cnt();

private:
	io_evt_notifier_ptr_t m_pIoEvtNotifier; ///the io event notify mechanism obj;
	int32_t m_i32id;
	//nm_utils::CEventHandleEngine m_evtengine; ///serialized the handles.
};
typedef nm_utils::CSmartPtr<nm_engine::CInputHandleTask> input_handle_task_ptr_t;


/**
 * output handler
 * */
class COutputHandleTask: public nm_thread::IThreadTask
{
public:
	COutputHandleTask();
	virtual ~COutputHandleTask();

public:
	///
	int32_t init(int32_t i32ioevtnotifier, int32_t i32MStimeout, int32_t i32id);
	int32_t destroy();
	///
	void exec();
	///
	void pos_evt(const nm_utils::event_ptr_t &pevt);
	int32_t add_io_obj(const io_obj_ptr_t &pioobj);
	int32_t del_io_obj(const io_obj_ptr_t &pioobj);
	///
	int32_t get_ioobj_cnt();

private:
	io_evt_notifier_ptr_t m_pIoEvtNotifier; ///the io event notify mechanism obj;
	int32_t m_i32id;
	nm_utils::CEventHandleEngine m_evtengine; ///serialized the handles.
};
typedef nm_utils::CSmartPtr<nm_engine::COutputHandleTask> output_handle_task_ptr_t;


}

#endif /* IO_TASK_H_ */
