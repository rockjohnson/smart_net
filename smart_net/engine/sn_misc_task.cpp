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
	// TODO Auto-generated destructor stub
}

int32_t CMiscTask::init(int32_t i32ioevtnotifier, int32_t i32MStimeout)
{
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
	m_pioevtnotifier->destroy();
	m_pioevtnotifier = NULL;

	m_ptimenotifier->destroy();
	m_pioevtnotifier = NULL;

	return CMNERR_SUC;
}

int32_t CMiscTask::add_io_obj(const io_obj_ptr_t &pioobj)
{
	SYS_ASSERT(pioobj->get_misc_evts() != 0);
	SYS_ASSERT(NULL != m_pioevtnotifier);

	m_pioevtnotifier->add_io_obj(pioobj, pioobj->get_misc_evts());
}

int32_t CMiscTask::del_io_obj(const io_obj_ptr_t &pioobj)
{

}

void CMiscTask::exec()
{
	while (!is_stopped())
	{
		///update io set




	}
}

}
