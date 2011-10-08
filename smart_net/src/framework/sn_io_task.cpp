/*
 * io_task.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#include "sn_io_task.h"

namespace nm_engine
{

CInputHandleTask::CInputHandleTask()
:m_i32id(-1)
{
}

CInputHandleTask::~CInputHandleTask()
{
	destroy();
}

int32_t CInputHandleTask::init(int32_t i32ioevtnotifier, int32_t i32MsTimeout, int32_t i32id)
{
	IF_TRUE_THEN_RETURN_CODE(NULL != m_pIoEvtNotifier, CMNERR_COMMON_ERR);

	///create io event notify mechanism obj.
	m_pIoEvtNotifier = IIoEvtNotifier::create_obj(i32ioevtnotifier);
	if (NULL == m_pIoEvtNotifier)
	{
		return CMNERR_COMMON_ERR;
	}

//	SYS_ASSERT(m_setIoObjAddCache.empty());
//	SYS_ASSERT(m_setIoObjs.empty());
//	SYS_ASSERT(m_setIoObjDelCache.empty());

	m_i32id = i32id;

	return m_pIoEvtNotifier->init(EITT_HANDLE_INPUT_TASK, i32MsTimeout);
}

int32_t CInputHandleTask::destroy()
{
	IF_TRUE_THEN_RETURN_CODE(NULL == m_pIoEvtNotifier, CMNERR_COMMON_ERR);

	m_pIoEvtNotifier->destroy();
	m_pIoEvtNotifier = NULL;
	m_i32id = -1;

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

void CInputHandleTask::post_event(const nm_utils::event_ptr_t &pevt)
{
	m_evtengine.post_event(pevt);
}

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
	return m_pIoEvtNotifier->add_io_obj(pIoObj, pIoObj->get_input_evts());
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

	return m_pIoEvtNotifier->del_io_obj(pioobj);
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
	SYS_ASSERT(NULL != m_pIoEvtNotifier);

	///main logic circle
	while (!is_stopped())
	{
		///
		m_evtengine->exec();
		///
		if (m_pIoEvtNotifier->exec() < CMNERR_SUC)
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

int32_t COutputHandleTask::init(int32_t i32ioevtnotifier, int32_t i32MStimeout, int32_t i32id)
{
	IF_TRUE_THEN_RETURN_CODE(NULL != m_pIoEvtNotifier, CMNERR_COMMON_ERR);

	///create io event notify mechanism obj.
	m_pIoEvtNotifier = IIoEvtNotifier::create_obj(i32ioevtnotifier);
	IF_TRUE_THEN_RETURN_CODE(NULL == m_pIoEvtNotifier, CMNERR_COMMON_ERR);

	m_i32id = i32id;

	return m_pIoEvtNotifier->init(EITT_HANDLE_OUTPUT_TASK, i32MStimeout);
}

int32_t COutputHandleTask::destroy()
{
	IF_TRUE_THEN_RETURN_CODE(NULL == m_pIoEvtNotifier, CMNERR_COMMON_ERR);

	m_pIoEvtNotifier->destroy();
	m_pIoEvtNotifier = NULL;
	m_i32id = -1;
}

int32_t COutputHandleTask::add_io_obj(const io_obj_ptr_t &pIoObj)
{
	SYS_ASSERT(NULL != m_pIoEvtNotifier);

	return m_pIoEvtNotifier->add_io_obj(pIoObj, pIoObj->get_output_evts());
}

int32_t COutputHandleTask::del_io_obj(const io_obj_ptr_t &pioobj)
{
	SYS_ASSERT(NULL != m_pIoEvtNotifier);

	return m_pIoEvtNotifier->del_io_obj(pioobj);
}

void COutputHandleTask::exec()
{
	while(!is_stopped())
	{
		///
		m_evtengine->exec();
		///
		m_pIoEvtNotifier->exec();
	}
}


}
