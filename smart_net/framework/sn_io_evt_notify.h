/*
 * io_evt_notify.h
 *
 *  Created on: Dec 24, 2010
 *      Author: rock
 */

#ifndef _IO_EVT_NOTIFY_H___
#define _IO_EVT_NOTIFY_H___

#include "sn_io_obj.h"

#if __PLATFORM__ == __PLATFORM_LINUX__
#include <sys/epoll.h>
#endif

namespace nm_framework
{

/**
 * io event notify mechanism base class
 * */
class IIoEvtNotify: public nm_utils::CSmartBase
{
public:
	enum EIoEvtNotify
	{
		EIEN_NONE = 0,
		EIEN_SELECT,
#if __PLATFORM__ == __PLATFORM_LINUX__
		EIEN_EPOLL,
#endif
		EIEN_ALL
	};

	static nm_utils::CSmartPtr<nm_framework::IIoEvtNotify> create_obj(int32_t i32IoEvtNotify);

public:
	IIoEvtNotify();
	virtual ~IIoEvtNotify();

public:
	virtual int32_t init(int32_t i32MsTimeout) = 0;
	virtual int32_t add_io_obj(io_obj_ptr_t &pIoObj) = 0;
	virtual int32_t del_io_obj(io_obj_ptr_t &pIoObj) = 0;
	virtual int32_t dispatch_evts() = 0;
};
typedef nm_utils::CSmartPtr<nm_framework::IIoEvtNotify> io_evt_notify_ptr_t;

/**
 * select mechanism
 * */
class CSelect: public IIoEvtNotify
{
public:
	CSelect();
	virtual ~CSelect();

public:
	int32_t exec();
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
	int32_t init(int32_t i32MsTimeout);
	int32_t add_io_obj();
	int32_t del_io_obj();
	int32_t dispatch_evts();

private:
	///
	int32_t mi32epfd;
	///
#define MAX_EVENTS (10000)
	struct epoll_event m_tmpEvts[MAX_EVENTS];
	//
	int32_t m_i32MsTimeout;
};

}

#endif /* __IO_EVT_NOTIFY_H__ */
