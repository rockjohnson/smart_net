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
		// TODO Auto-generated destructor stub
	}

	int32_t CIoTask::init(int32_t i32IoEvtNofify)
	{
		///create io event notify mechanism obj.
		m_pIoEvtNotify = IIoEvtNotify::create_obj(i32IoEvtNofify);
		if (NULL == m_pIoEvtNotify)
		{
			return CMNERR_COMMON_ERR;
		}

		return m_pIoEvtNotify->init();
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
		if (0 > m_pIoEvtNotify->dispatch_evts())
		{
			///do something.
		}
	}

}
