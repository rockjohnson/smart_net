/*
 * sn_io_evt_notifier.h
 *
 *  Created on: Mar 10, 2011
 *      Author: rock
 */

#ifndef __SN_IO_EVT_NOTIFIER_H__
#define __SN_IO_EVT_NOTIFIER_H__

namespace nm_engine
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
class IIoEvtNotifier: public nm_base::ICommonBase
{

public:
	IIoEvtNotifier();
	virtual ~IIoEvtNotifier();

public:
	///
	virtual int32_t init(int32_t i32MsTimeout) = 0;
	virtual int32_t destroy() = 0;
	///
	virtual int32_t add_io_obj(const io_obj_ptr_t &pIoObj, u_int32_t ui32Evts) = 0;
	virtual int32_t del_io_obj(const io_obj_ptr_t &pIoObj) = 0;
	virtual int32_t dispatch_evts() = 0;
};
typedef nm_utils::CSmartPtr<IIoEvtNotifier> ioevt_notifier_ptr_t;


}


#endif /* __SN_IO_EVT_NOTIFIER_H__ */
