/*
 * io_evt_notify.h
 *
 *  Created on: Dec 24, 2010
 *      Author: rock
 */

#ifndef _IO_EVT_NOTIFY_H___
#define _IO_EVT_NOTIFY_H___


#if __PLATFORM__ == __PLATFORM_LINUX__
#include <sys/epoll.h>
#endif

#include <set>
#include <vector>

#include <utils/obj_factory.h>
#include <utils/smart_lock.h>

#include "../framework/sn_io_evt_notifier.h"

namespace nm_event
{

class CIoEvtNotifierFactory : public nm_utils::CObjFactory<nm_framework::IIoEvtNotifier>
{

public:
	virtual obj_ptr_t& create_obj(int32_t i32Type);
};

/**
 * select mechanism
 * */
//class CSelect: public nm_framework::IIoEvtNotifier
//{
//public:
//	CSelect();
//	virtual ~CSelect();
//
//public:
//	///
//	virtual int32_t init(int32_t i32MsTimeout);
//	virtual int32_t destroy();
//	///
//	virtual int32_t add_io_obj(const io_obj_ptr_t &pIoObj);
//	virtual int32_t del_io_obj(const io_obj_ptr_t &pIoObj);
//	virtual int32_t exec();
//};

/**
 * epoll mechanism
 * */
class CEpoll: public nm_framework::IIoEvtNotifier
{
public:
	CEpoll();
	virtual ~CEpoll();

	DISALLOW_COPY_AND_ASSIGN(CEpoll);

public:
	///
	int32_t init(int32_t i32IoTaskType, int32_t i32MsTimeout);
	int32_t destroy();
	///
	int32_t add_io_obj(const nm_framework::io_obj_ptr_t &pIoObj);
	int32_t del_io_obj(const nm_framework::io_obj_ptr_t &pIoObj);
	///
	int32_t exec();

private:
	///
	int32_t m_i32epfd;
	///
	int32_t m_i32IoType;
#define MAX_EVENTS (10000)
	struct epoll_event m_arrEvts[MAX_EVENTS];
	///
	int32_t m_i32MsTimeout;
	///
	typedef std::vector<nm_framework::io_obj_ptr_t> io_obj_vec_t;
	typedef std::set<nm_framework::io_obj_ptr_t> io_obj_set_t;
	io_obj_set_t m_setIoObjs;
	///
	io_obj_vec_t m_vecIoObjsAddCache;
	nm_utils::CSpinLock m_lkIoObjAddCache;
	io_obj_vec_t m_vecIoObjsDelCache;
	nm_utils::CSpinLock m_lkIoObjDelCache;
};

}

#endif /* __IO_EVT_NOTIFY_H__ */
