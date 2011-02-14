/*
 * io_task.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#include "sn_io_task.h"

namespace nm_framework
{

	CIoTask::CIoTask()
	{
		// TODO Auto-generated constructor stub
	}

	CIoTask::~CIoTask()
	{
		destroy();
	}

	int32_t CIoTask::init(int32_t i32IoEvtNofify)
	{
		///create io event notify mechanism obj.
		m_pIoEvtNotifier = IIoEvtNotify::create_obj(i32IoEvtNofify);
		if (NULL == m_pIoEvtNotifier)
		{
			return CMNERR_COMMON_ERR;
		}

		return m_pIoEvtNotifier->init();
	}

	int32_t CIoTask::destroy()
	{
		if (NULL != m_pIoEvtNotifier)
		{
			m_pIoEvtNotifier->destroy();
		}

		///io obj add cache
		io_obj_set_t m_setIoObjCache;
		CSpinLock m_splkIoObjCache;
		///the current valid io objs
		io_obj_set_t m_setValidIoObjs;
		///...
		io_obj_set_t m_setInvalidIoObjs;
		CSpinLock m_splkInvalidIoObjs;
	}

	void CIoTask::exec()
	{
		IF_TRUE_THEN_RETURN(init() < 0);

		///main logic circle
		while (!is_stopped())
		{
			///update internal io set.
			update_internal_io_set();

			///handle io events.
			handle_io_evts();

			///handle internal timers.
			handle_internal_timers();

			///handle bad io objs.
			handle_bad_io_objs();
		}
	}

	void CIoTask::handle_io_evts()
	{
		if (0 > m_pIoEvtNotifier->dispatch_evts())
		{
			///do something.
		}
	}

}
