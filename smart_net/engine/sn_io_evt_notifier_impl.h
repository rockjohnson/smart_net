/*
 * io_evt_notify.h
 *
 *  Created on: Dec 24, 2010
 *      Author: rock
 */

#ifndef _IO_EVT_NOTIFY_H___
#define _IO_EVT_NOTIFY_H___

#include "sn_io_evt_notifier.h"

#if __PLATFORM__ == __PLATFORM_LINUX__
#include <sys/epoll.h>
#endif

namespace nm_engine
{

/**
 * select mechanism
 * */
class CSelect: public IIoEvtNotify
{
public:
	CSelect();
	virtual ~CSelect();

public:
	///
	virtual int32_t init(int32_t i32MsTimeout);
	virtual int32_t destroy();
	///
	virtual int32_t add_io_obj(const io_obj_ptr_t &pIoObj);
	virtual int32_t del_io_obj(const io_obj_ptr_t &pIoObj);
	virtual int32_t dispatch_evts();
};

/**
 * epoll mechanism
 * */
class CEpoll: public IIoEvtNotify
{
public:
	CEpoll();
	virtual ~CEpoll();

public:
	///
	int32_t init(int32_t i32MsTimeout);
	int32_t destroy();
	///
	int32_t add_io_obj(const io_obj_ptr_t &pIoObj, u_int32_t ui32Evts);
	int32_t del_io_obj(const io_obj_ptr_t &pIoObj);
	int32_t dispatch_evts();

private:
	///
	int32_t m_i32epfd;
	///
#define MAX_EVENTS (10000)
	struct epoll_event m_tmpEvts[MAX_EVENTS];
	//
	int32_t m_i32MsTimeout;
};

}

#endif /* __IO_EVT_NOTIFY_H__ */
