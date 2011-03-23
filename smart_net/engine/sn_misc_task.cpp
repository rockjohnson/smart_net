/*
 * sn_misc_task.cpp
 *
 *  Created on: Mar 8, 2011
 *      Author: rock
 */

#include "sn_misc_task.h"

#include "sn_io_evt_notifier_impl.h"

namespace nm_engine
{

CMiscTask::CMiscTask()
{
	// TODO Auto-generated constructor stub

}

CMiscTask::~CMiscTask()
{
	destroy();
}

int32_t CMiscTask::init(int32_t i32ioevtnotifier, int32_t i32MStimeout)
{
	IF_TRUE_THEN_RETURN_CODE(NULL != m_pioevtnotifier || NULL != m_ptimenotifier, CMNERR_COMMON_ERR);

	CIoEvtNotifierFactory fac;
	m_pioevtnotifier = fac.create_obj(i32ioevtnotifier);
	IF_TRUE_THEN_RETURN_CODE(NULL == m_pioevtnotifier, CMNERR_COMMON_ERR);
	int32_t i32ret = m_pioevtnotifier->init(i32MStimeout);
	IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC > i32ret, i32ret);

	m_ptimenotifier = SYS_NOTRW_NEW(CTimeNotifier);
	IF_TRUE_THEN_RETURN_CODE(NULL == m_ptimenotifier, CMNERR_COMMON_ERR);
	i32ret = m_pioevtnotifier->init();
	IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC > i32ret, i32ret);

	return i32ret;
}

int32_t CMiscTask::destroy()
{
	if (NULL != m_pioevtnotifier)
	{
		m_pioevtnotifier->destroy();
		m_pioevtnotifier = NULL;
	}

	if (NULL != m_ptimenotifier)
	{
		m_ptimenotifier->destroy();
		m_pioevtnotifier = NULL;
	}

	return CMNERR_SUC;
}

int32_t CMiscTask::add_io_obj(const io_obj_ptr_t &pioobj)
{
	SYS_ASSERT(pioobj->get_misc_evts() != 0);
	SYS_ASSERT(NULL != m_pioevtnotifier);

	return m_pioevtnotifier->add_io_obj(pioobj, pioobj->get_misc_evts());
}

int32_t CMiscTask::del_io_obj(const io_obj_ptr_t &pioobj)
{
	SYS_ASSERT(pioobj->get_misc_evts() != 0);
	SYS_ASSERT(NULL != m_pioevtnotifier);

	return m_pioevtnotifier->del_io_obj(pioobj);
}

int32_t CMiscTask::add_timer(const internal_timer_ptr_t &ptimer)
{
	SYS_ASSERT(NULL != m_ptimenotifier);

	return m_ptimenotifier->add_timer(ptimer);
}

int32_t CMiscTask::del_timer(const internal_timer_ptr_t &ptimer)
{
	SYS_ASSERT(NULL != m_ptimenotifier);

	return m_ptimenotifier->del_timer(ptimer);
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
		m_evtengine.exec();

		///handle events.
		m_pioevtnotifier->exec();

		///timers.
		m_ptimenotifier->exec();
	}
}

}
