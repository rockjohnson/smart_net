/*
 * sn_misc_task.cpp
 *
 *  Created on: Mar 8, 2011
 *      Author: rock
 */

#include "sn_misc_task.h"

//#include "../event/sn_io_evt_notifier_impl.h"

namespace nm_framework
{

CMiscTask::CMiscTask()
{
	// TODO Auto-generated constructor stub

}

CMiscTask::~CMiscTask()
{
	destroy();
}

int32_t CMiscTask::init()
{
	IF_TRUE_THEN_RETURN_CODE(NULL != m_pTimeNotifier, CMNERR_COMMON_ERR);

//	nm_event::CIoEvtNotifierFactory fac;
//	m_pIoEvtNotifier = fac.create_obj(i32ioevtnotifier);
//	IF_TRUE_THEN_RETURN_CODE(NULL == m_pIoEvtNotifier, CMNERR_COMMON_ERR);
//	int32_t i32ret = m_pIoEvtNotifier->init(i32MStimeout);
//	IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC > i32ret, i32ret);

	m_pTimeNotifier = SYS_NOTRW_NEW(CTimeNotifier);
	IF_TRUE_THEN_RETURN_CODE(NULL == m_pTimeNotifier, CMNERR_COMMON_ERR);
	int32_t i32Ret = m_pTimeNotifier->init();
	IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC > i32Ret, i32Ret);

	return i32Ret;
}

int32_t CMiscTask::destroy()
{
//	if (NULL != m_pIoEvtNotifier)
//	{
//		m_pIoEvtNotifier->destroy();
//		m_pIoEvtNotifier = NULL;
//	}

	if (NULL != m_pTimeNotifier)
	{
		m_pTimeNotifier->destroy();
		//m_pIoEvtNotifier = NULL;
	}

	return CMNERR_SUC;
}

//int32_t CMiscTask::add_io_obj(const io_obj_ptr_t &pioobj)
//{
//	SYS_ASSERT(pioobj->get_misc_evts() != 0);
//	//SYS_ASSERT(NULL != m_pIoEvtNotifier);
//
//	return m_pIoEvtNotifier->add_io_obj(pioobj, pioobj->get_misc_evts());
//}

//int32_t CMiscTask::del_io_obj(const io_obj_ptr_t &pioobj)
//{
//	SYS_ASSERT(pioobj->get_misc_evts() != 0);
//	SYS_ASSERT(NULL != m_pIoEvtNotifier);
//
//	return m_pIoEvtNotifier->del_io_obj(pioobj);
//}

int32_t CMiscTask::add_timer(const timer_obj_ptr_t &ptimer)
{
	SYS_ASSERT(NULL != m_pTimeNotifier);

	return m_pTimeNotifier->add_timer(ptimer);
}

int32_t CMiscTask::del_timer(const timer_obj_ptr_t &ptimer)
{
	SYS_ASSERT(NULL != m_pTimeNotifier);

	return m_pTimeNotifier->del_timer(ptimer);
}

void CMiscTask::post_evt(nm_utils::event_ptr_t &pevt)
{
	return m_evtengine.post_event(pevt);
}

void CMiscTask::exec()
{
	while (!is_stopped())
	{
		///handle evt
//		m_evtengine.exec();

//		///handle events.
//		m_pIoEvtNotifier->exec();

		///timers.
		m_pTimeNotifier->exec();

		///
		::sleep(0);
	}
}

}
