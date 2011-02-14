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

namespace nm_framework
{
	using namespace nm_utils;

	/**
	 *
	 * */
	class CIoTask : public nm_thread::IThreadTask
	{
		public:
			CIoTask();
			virtual ~CIoTask();

		public:
			void exec();
			int32_t add_io_obj(io_obj_ptr_t &pIoObj, u_int32_t ui32Evts);
			int32_t del_io_obj(io_obj_ptr_t &pIoObj);

		private:
			io_evt_notify_ptr_t m_pIoEvtNotify; ///the io event notify mechanism obj;
			typedef std::set<nm_framework::io_obj_ptr_t> io_obj_set_t;
			///io obj add cache
			io_obj_set_t m_setIoObjCache;
			CSpinLock m_splkIoObjCache;
			///the current valid io objs
			io_obj_set_t m_setValidIoObjs;
			///...
			io_obj_set_t m_setInvalidIoObjs;
			CSpinLock m_splkInvalidIoObjs;
			///max io wait interval.
			u_int32_t m_ui32MaxIoEvtWaitInterval;
	};

	typedef nm_utils::CSmartPtr<nm_framework::CIoTask> io_task_ptr_t;

}

#endif /* IO_TASK_H_ */
