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

	int32_t CIoTask::init(int32_t i32IoEvtNotifier, int32_t i32MsTimeout)
	{
		///create io event notify mechanism obj.
		m_pIoEvtNotifier = IIoEvtNotify::create_obj(i32IoEvtNotifier);
		if (NULL == m_pIoEvtNotifier)
		{
			return CMNERR_COMMON_ERR;
		}

		SYS_ASSERT(m_setIoObjCache.empty());
		SYS_ASSERT(m_setValidIoObjs.empty());
		SYS_ASSERT(m_setInvalidIoObjs.empty());

		return m_pIoEvtNotifier->init(i32MsTimeout);
	}

	int32_t CIoTask::destroy()
	{
		if (NULL != m_pIoEvtNotifier)
		{
			m_pIoEvtNotifier->destroy();
		}

		///io obj add cache
		m_splkIoObjCache.lock();
		m_setIoObjCache.clear();
		m_splkIoObjCache.unlock();

		///
		m_setValidIoObjs.clear();

		///...
		m_splkInvalidIoObjs.lock();
		m_setInvalidIoObjs.clear();
		m_splkInvalidIoObjs.unlock();

		return CMNERR_SUC;
	}

	void CIoTask::exec()
	{
		///main logic circle
		while (!is_stopped())
		{
			///update internal io set.
			update_internal_ioset();

			///handle io events.
			handle_io_evts();

			///handle internal timers.
			handle_internal_timers();

			///handle bad io objs.
			handle_bad_ioobjs();
		}
	}

	void CIoTask::handle_io_evts()
	{
		if (0 > m_pIoEvtNotifier->dispatch_evts())
		{
			///do something.
		}
	}

	void handle_bad_io_objs()
	{

	}

}
