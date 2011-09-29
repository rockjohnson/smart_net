/*
 * io_evt_notify.cpp
 *
 *  Created on: Dec 24, 2010
 *      Author: rock
 */

#include "sn_io_evt_notifier_impl.h"

namespace nm_engine
{

IIoEvtNotifier::IIoEvtNotifier()
{
	// TODO Auto-generated constructor stub
}

IIoEvtNotifier::~IIoEvtNotifier()
{
	destroy();
}

io_evt_notifier_ptr_t& CIoEvtNotifierFactory::create_obj(
		int32_t i32ioevtnotifier)
{
	io_evt_notifier_ptr_t pioevtnotifier = NULL;

	if (EIEN_ALL <= i32ioevtnotifier || EIEN_NONE >= i32ioevtnotifier)
	{
		return pioevtnotifier;
	}

	switch (i32ioevtnotifier)
	{
	case EIEN_SELECT:
	{
		pioevtnotifier = SYS_NOTRW_NEW(CSelect);
		break;
	}
#if __PLATFORM__ == __PLATFORM_LINUX__
	case EIEN_EPOLL:
	{
		pioevtnotifier = SYS_NOTRW_NEW(CEpoll);
		break;
	}
#endif
	default:
	{
		break;
	}
	}

	return pioevtnotifier;
}

/*
 * select ..
 * */
CSelect::CSelect()
{
	// TODO Auto-generated constructor stub
}

CSelect::~CSelect()
{
	// TODO Auto-generated destructor stub
}

/*
 * epoll ...
 * */
CEpoll::CEpoll() :
	m_i32epfd(-1), m_i32MSTimeout(0)
{
	ZERO_MEM(&m_arrEvts, sizeof(m_arrEvts));
}

CEpoll::~CEpoll()
{
	// TODO Auto-generated destructor stub
	//destroy();
}

#define UNUSED (10000) //Since Linux 2.6.8, the size argument is unused.  (The kernel dynamically sizes the required data structures without needing this initial hint.)
int32_t CEpoll::init(int32_t i32MsTimeout)
{
	SYS_ASSERT(-1 == m_i32epfd);
	SYS_ASSERT(m_setIoObjsAddCache.empty());
	SYS_ASSERT(m_setIoObjsDelCache.empty());
	SYS_ASSERT(m_setIoObjs.empty());

	m_i32MSTimeout = i32MsTimeout;

	return (m_i32epfd = epoll_create(UNUSED)) < 0 ? CMNERR_COMMON_ERR
			: CMNERR_SUC;
}

int32_t CEpoll::destroy()
{
	SYS_ASSERT(0 <= m_i32epfd);

	nm_utils::spin_scopelk_t lk(m_lkIoObjCache);
	m_setIoObjsAddCache.clear();
	m_setIoObjsDelCache.clear();
	m_setIoObjs.clear(); ///not thread safe

	return CMNERR_COMMON_ERR;
}

int32_t CEpoll::add_io_obj(const io_obj_ptr_t &pIoObj, u_int32_t ui32evts)
{
	struct epoll_event evt;
	evt.events = ui32Evts;
	evt.data.ptr = pIoObj.get_ptr();

	return epoll_ctl(m_i32epfd, EPOLL_CTL_ADD, pIoObj->get_handle(), &evt);
}

int32_t CEpoll::del_io_obj(const io_obj_ptr_t &pioobj)
{
	if (pioobj->get_handle() >= 0)
	{
		struct epoll_event evt;
		epoll_ctl(m_i32epfd, EPOLL_CTL_DEL, pioobj->get_handle(), &evt);
	}

	{
		nm_utils::spin_scopelk_t lk(m_lkIoObjCache);

	}
}

int32_t CEpoll::exec()
{
	///del io obj from epoll set, the
	{
		CS
	}

	///add io obj into epoll set

	///wait io event...
	int32_t i32Ret = epoll_wait(m_i32epfd, m_arrEvts, MAX_EVENTS, m_i32MSTimeout);
	if (-1 == i32Ret)
	{
		SYS_ASSERT(false);
		return CMNERR_FATAL_ERR;
	}

	///process io evt...
	IIoObj *pIoObj = NULL;
	u_int32_t ui32Evts = 0;
	for (int i = 0; i < i32Ret; i++)
	{
		pIoObj = static_cast<IIoObj*> (m_arrEvts[i].data.ptr); ///no need add ref, for i have add it outside.
		ui32Evts = m_arrEvts[i].events;

		//error
		if (ui32Evts & EPOLLERR)
		{
			SYS_ASSERT(false); ///check how this happened
			pIoObj->handle_error_evt();
		}
		else
		{
			if (ui32Evts & EPOLLIN) ///input
			{
				pIoObj->handle_input_evt();
			}
			else if (ui32Evts & EPOLLOUT) ///output
			{
				pIoObj->handle_output_evt();
			}
			else
			{
				SYS_ASSERT(false);
			}
		}
	}

	return CMNERR_SUC;
}

}
