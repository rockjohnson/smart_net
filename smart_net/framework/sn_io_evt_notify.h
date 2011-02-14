/*
 * io_evt_notify.h
 *
 *  Created on: Dec 24, 2010
 *      Author: rock
 */

#ifndef _IO_EVT_NOTIFY_H___
#define _IO_EVT_NOTIFY_H___

#include "io_obj.h"

namespace nm_framework
{

	/**
	 * io event notify mechanism base class
	 * */
	class IIoEvtNotify : public nm_utils::CSmartBase
	{
		public:
			IIoEvtNotify();
			virtual ~IIoEvtNotify();

		public:
			virtual int32_t init() = 0;
			virtual int32_t add_io_obj() = 0;
			virtual int32_t del_io_obj() = 0;
			virtual int32_t exec() = 0;
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
	class CEpoll
	{
		public:
			CEpoll();
			virtual ~CEpoll();
	};

}

#endif /* __IO_EVT_NOTIFY_H__ */
