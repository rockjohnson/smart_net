/*
 * io_task.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#include "sn_io_task.h"

namespace nm_engine
{

CIoTask::CIoTask()
{
	// TODO Auto-generated constructor stub
}

CIoTask::~CIoTask()
{
	destroy();
}

int32_t CIoTask::init(int32_t i32IoEvtNotifier, int32_t i32MsTimeout, int32_t i32ID)
{
	///create io event notify mechanism obj.
	m_pIoEvtNotifier = IIoEvtNotify::create_obj(i32IoEvtNotifier);
	if (NULL == m_pIoEvtNotifier)
	{
		return CMNERR_COMMON_ERR;
	}

	SYS_ASSERT(m_setIoObjAddCache.empty());
	SYS_ASSERT(m_setIoObjs.empty());
	SYS_ASSERT(m_setIoObjDelCache.empty());

	m_i32ID = i32ID;

	return m_pIoEvtNotifier->init(i32MsTimeout);
}

int32_t CIoTask::destroy()
{
	if (NULL != m_pIoEvtNotifier)
	{
		m_pIoEvtNotifier->destroy();
	}

	///io obj add cache
	m_lkIoObjCache.lock();
	m_setIoObjAddCache.clear();
	m_lkIoObjCache.unlock();
	///
	m_setIoObjs.clear();
	///...
	m_splkInvalidIoObjs.lock();
	m_setIoObjDelCache.clear();
	m_splkInvalidIoObjs.unlock();

	return CMNERR_SUC;
}

int32_t CIoTask::add_io_obj(const io_obj_ptr_t &pIoObj)
{
	spin_scopelk_t lk(m_lkIoObjCache);

	///check del cache
	io_obj_set_ret_t ret = m_setIoObjDelCache.find(pIoObj);
	SYS_ASSERT(!ret.second);

	///check io set
	ret = m_setIoObjs.find(pIoObj);
	SYS_ASSERT(!ret.second);

	ret = m_setIoObjAddCache.insert(pIoObj);

	return ret.second ? CMNERR_SUC : CMNERR_COMMON_ERR;
}

/**
 *
 * */
int32_t CIoTask::del_io_obj(const io_obj_ptr_t &pIoObj)
{
	spin_scopelk_t lk(m_lkIoObjCache);

	///check add cache
	io_obj_set_ret_t ret = m_setIoObjAddCache.find(pIoObj);
	SYS_ASSERT(!ret.second);

	///check io set
	ret = m_setIoObjs.find(pIoObj);
	SYS_ASSERT(ret.second);

	ret = m_setIoObjDelCache.insert(pIoObj);

	return ret.second ? CMNERR_SUC : CMNERR_COMMON_ERR;
}

void CIoTask::update_io_set()
{
	if (m_setIoObjDelCache.empty() && m_setIoObjAddCache.empty())
	{
		return;
	}

	{
		io_obj_set_ret_t ret;
		spin_scopelk_t lk(m_lkIoObjCache);

		///delete first
		for (io_obj_set_t::iterator iter = m_setIoObjDelCache.begin(); iter
				!= m_setIoObjDelCache.end(); iter++)
		{
			ret = m_setIoObjs.erase(*iter);
			SYS_ASSERT(ret.second);
		}
		m_setIoObjDelCache.clear();

		///then, insert.
		for (io_obj_set_t::iterator iter = m_setIoObjAddCache.begin(); iter
				!= m_setIoObjAddCache.end(); iter++)
		{
			ret = m_setIoObjs.insert(*iter);
			SYS_ASSERT(ret.second);
		}
		m_setIoObjAddCache.clear();
	}
}

void CIoTask::exec()
{
	///main logic circle
	while (!is_stopped())
	{
		///update internal io set.
		update_io_set();

		///handle io events.
		handle_io_evts();
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
