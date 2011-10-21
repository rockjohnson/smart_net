/*
 * io_task.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#include "sn_io_task.h"
#include "../event/sn_io_evt_notifier_impl.h"

namespace nm_framework
{

CInputHandleTask::CInputHandleTask()
:m_i32TaskId(-1)
{
}

CInputHandleTask::~CInputHandleTask()
{
	destroy();
}

int32_t CInputHandleTask::init(int32_t i32ioevtnotifier, int32_t i32MsTimeout)
{
	IF_TRUE_THEN_RETURN_CODE(NULL != m_pIoEvtNotifier, CMNERR_COMMON_ERR);

	///create io event notify mechanism obj.
	nm_event::CIoEvtNotifierFactory IoEvtFac;
	m_pIoEvtNotifier = IoEvtFac.create_obj(i32ioevtnotifier);
	if (NULL == m_pIoEvtNotifier)
	{
		return CMNERR_COMMON_ERR;
	}

	return m_pIoEvtNotifier->init(EIT_INPUT_TYPE, i32MsTimeout);
}

void CInputHandleTask::set_id(int32_t i32Indx)
{
	m_i32TaskId = i32Indx;
}

int32_t CInputHandleTask::destroy()
{
	IF_TRUE_THEN_RETURN_CODE(NULL == m_pIoEvtNotifier, CMNERR_COMMON_ERR);

	m_pIoEvtNotifier->destroy();
	m_pIoEvtNotifier = NULL;
	m_i32TaskId = -1;

//	///io obj add cache
//	m_lkIoObjCache.lock();
//	m_setIoObjAddCache.clear();
//	m_lkIoObjCache.unlock();
//	///
//	m_setIoObjs.clear();
//	///...
//	m_splkInvalidIoObjs.lock();
//	m_setIoObjDelCache.clear();
//	m_splkInvalidIoObjs.unlock();

	return CMNERR_SUC;
}

//void CInputHandleTask::post_event(const nm_utils::event_ptr_t &pevt)
//{
//	m_evtengine.post_event(pevt);
//}

int32_t CInputHandleTask::add_io_obj(const io_obj_ptr_t &pIoObj)
{
//	spin_scopelk_t lk(m_lkIoObjCache);
//
//	///check del cache
//	io_obj_set_ret_t ret = m_setIoObjDelCache.find(pIoObj);
//	SYS_ASSERT(!ret.second);
//
//	///check io set
//	ret = m_setIoObjs.find(pIoObj);
//	SYS_ASSERT(!ret.second);
//
//	ret = m_setIoObjAddCache.insert(pIoObj);

//	return ret.second ? CMNERR_SUC : CMNERR_COMMON_ERR;
	pIoObj->set_task_id(EIT_INPUT_TYPE, m_i32TaskId);
	return m_pIoEvtNotifier->add_ioobj(pIoObj);
}

/**
 *
 * */
int32_t CInputHandleTask::del_io_obj(const io_obj_ptr_t &pioobj)
{
//	spin_scopelk_t lk(m_lkIoObjCache);
//
//	///check add cache
//	io_obj_set_ret_t ret = m_setIoObjAddCache.find(pIoObj);
//	SYS_ASSERT(!ret.second);
//
//	///check io set
//	ret = m_setIoObjs.find(pIoObj);
//	SYS_ASSERT(ret.second);
//
//	ret = m_setIoObjDelCache.insert(pIoObj);
//
//	return ret.second ? CMNERR_SUC : CMNERR_COMMON_ERR;

	return m_pIoEvtNotifier->del_ioobj(pioobj);
}

//void CInputHandleTask::update_io_set()
//{
//	if (m_setIoObjDelCache.empty() && m_setIoObjAddCache.empty())
//	{
//		return;
//	}
//
//	{
//		io_obj_set_ret_t ret;
//		spin_scopelk_t lk(m_lkIoObjCache);
//
//		///delete first
//		for (io_obj_set_t::iterator iter = m_setIoObjDelCache.begin(); iter
//				!= m_setIoObjDelCache.end(); iter++)
//		{
//			ret = m_setIoObjs.erase(*iter);
//			SYS_ASSERT(ret.second);
//		}
//		m_setIoObjDelCache.clear();
//
//		///then, insert.
//		for (io_obj_set_t::iterator iter = m_setIoObjAddCache.begin(); iter
//				!= m_setIoObjAddCache.end(); iter++)
//		{
//			ret = m_setIoObjs.insert(*iter);
//			SYS_ASSERT(ret.second);
//		}
//		m_setIoObjAddCache.clear();
//	}
//}

void CInputHandleTask::exec()
{
	CMN_ASSERT(NULL != m_pIoEvtNotifier);

	///main logic circle
	while (!is_stopped())
	{
		///
		if (m_pIoEvtNotifier->exec() < 0)
		{
			break;
		}
	}
}


/**
 * output handle
 * */
COutputHandleTask::COutputHandleTask()
{
}

COutputHandleTask::~COutputHandleTask()
{
	destroy();
}

void COutputHandleTask::pos_evt(const nm_utils::event_ptr_t &pevt)
{
	m_evtengine.post_event(pevt);
}

void COutputHandleTask::set_indx(int32_t i32Indx)
{
	m_i32TaskId = i32Indx;
}

int32_t COutputHandleTask::init(int32_t i32IoEvtNotifier, int32_t i32MStimeout)
{
	IF_TRUE_THEN_RETURN_CODE(NULL != m_pIoEvtNotifier, CMNERR_COMMON_ERR);

	///create io event notify mechanism obj.
	nm_event::CIoEvtNotifierFactory fac;
	m_pIoEvtNotifier = fac.create_obj(i32IoEvtNotifier);
	IF_TRUE_THEN_RETURN_CODE(NULL == m_pIoEvtNotifier, CMNERR_COMMON_ERR);

	return m_pIoEvtNotifier->init(EIT_OUTPUT_TYPE, i32MStimeout);
}

int32_t COutputHandleTask::destroy()
{
	IF_TRUE_THEN_RETURN_CODE(NULL == m_pIoEvtNotifier, CMNERR_COMMON_ERR);

	m_pIoEvtNotifier->destroy();
	m_pIoEvtNotifier = NULL;
	m_i32TaskId = -1;
}

int32_t COutputHandleTask::add_io_obj(const io_obj_ptr_t &pIoObj)
{
	CMN_ASSERT(NULL != m_pIoEvtNotifier);

	pIoObj->set_task_id(EIT_OUTPUT_TYPE, m_i32TaskId);
	return m_pIoEvtNotifier->add_ioobj(pIoObj);
}

int32_t COutputHandleTask::del_io_obj(const io_obj_ptr_t &pioobj)
{
	CMN_ASSERT(NULL != m_pIoEvtNotifier);

	return m_pIoEvtNotifier->del_ioobj(pioobj);
}

void COutputHandleTask::exec()
{
	while(!is_stopped())
	{
		///
		if (m_pIoEvtNotifier->exec() < 0)
		{
			break;
		}
	}
}


}
