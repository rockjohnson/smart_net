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

	int32_t CIoTask::init()
	{
		///create io event notify mechanism obj.
#if __IO_NOTIFY_MECHANISM__ == __EPOLL__
		m_pIoEvtNotify = new CEpoll;
#elif __IO_NOTIFY_MECHANISM__ == __SELECT__
		m_pIoEvtNotify = new CSelect;
#endif
		int32_t iRet = m_pIoEvtNotify->init();
		if (0 > iRet)
		{
			TRACE_LAST_ERR(IO_LOG, m_pIoEvtNotify->init());
		}

		return iRet;
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
		setIoEvts.clear();
		iRet = m_pIoEvtNotify->wait_io_evts(setIoEvts, m_ui32MaxIoEvtWaitInterval);
		if (0 > iRet)
		{
			TRACE_LAST_ERR(IO_LOG, m_pIoEvtNotify->wait_io_evts());
			return;
		}
		if (setIoEvts.get_evt_cnt() > 0)
		{
			///handle the events.
			handle_io_evts(setIoEvts);
		}
	}

}