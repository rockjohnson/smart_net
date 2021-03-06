/*
 * sn_io_evt_notifier.h
 *
 *  Created on: Mar 10, 2011
 *      Author: rock
 */

#ifndef __SN_IO_EVT_NOTIFIER_H__
#define __SN_IO_EVT_NOTIFIER_H__

#include "sn_io_obj.h"

namespace nm_framework
{

enum EIoEvtNotify
{
	EIEN_NONE = 0,
	EIEN_SELECT,
#if __PLATFORM__ == __PLATFORM_LINUX__
	EIEN_EPOLL,
#endif
	EIEN_ALL
};


/**
 * io event notify mechanism base class
 * */
class IIoEvtNotifier: public nm_cmn_base::ICommonBase
{

public:
	IIoEvtNotifier(){}
	virtual ~IIoEvtNotifier(){}

public:
	///
	virtual int32_t init(int32_t i32IoTaskType, int32_t i32MsTimeout) = 0;
	virtual int32_t destroy() = 0;
	///
	virtual u_int32_t get_ioobj_cnt() = 0;
	virtual int32_t add_ioobj(const io_obj_ptr_t &pIoObj) = 0;
	virtual int32_t del_ioobj(const io_obj_ptr_t &pIoObj) = 0;
	virtual int32_t exec() = 0;
};
typedef nm_utils::CSmartPtr<IIoEvtNotifier> io_evt_notifier_ptr_t;


}


#endif /* __SN_IO_EVT_NOTIFIER_H__ */
