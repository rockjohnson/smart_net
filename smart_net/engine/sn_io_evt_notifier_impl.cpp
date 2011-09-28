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

ioevt_notifier_ptr_t& CIoEvtNotifierFactory::create_obj(
		int32_t i32ioevtnotifier)
{
	ioevt_notifier_ptr_t pioevtnotifier = NULL;

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
		pioevtnotifier = SYS_NOTRW_NEW(CEpollWrapper);
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
CEpollWrapper::CEpollWrapper() :
	m_i32epfd(-1), m_i32MSTimeout(0)
{
	ZERO_MEM(&m_tmpEvts, sizeof(m_tmpEvts));
}

CEpollWrapper::~CEpollWrapper()
{
	// TODO Auto-generated destructor stub
	//destroy();
}

#define UNUSED (10000) //Since Linux 2.6.8, the size argument is unused.  (The kernel dynamically sizes the required data structures without needing this initial hint.)
int32_t CEpollWrapper::init(int32_t i32MsTimeout)
{
	SYS_ASSERT(-1 == m_i32epfd);
	SYS_ASSERT(m_setioobjaddcache.empty());
	SYS_ASSERT(m_setioobjdelcache.empty());
	SYS_ASSERT(m_setioobjs.empty());

	m_i32MSTimeout = i32MsTimeout;

	return (m_i32epfd = epoll_create(UNUSED)) < 0 ? CMNERR_COMMON_ERR
			: CMNERR_SUC;
}

int32_t CEpollWrapper::destroy()
{
	SYS_ASSERT(0 <= m_i32epfd);

	nm_utils::spin_scopelk_t lk(m_lkioobjcache);
	m_setioobjaddcache.clear();
	m_setioobjdelcache.clear();
	m_setioobjs.clear(); ///not thread safe

	return CMNERR_COMMON_ERR;
}

int32_t CEpollWrapper::add_io_obj(const io_obj_ptr_t &pIoObj, u_int32_t ui32evts)
{
	struct epoll_event evt;
	evt.events = ui32Evts;
	evt.data.ptr = pIoObj.get_ptr();

	return epoll_ctl(m_i32epfd, EPOLL_CTL_ADD, pIoObj->get_handle(), &evt);
}

int32_t CEpollWrapper::del_io_obj(const io_obj_ptr_t &pioobj)
{
	if (pioobj->get_handle() >= 0)
	{
		struct epoll_event evt;
		epoll_ctl(m_i32epfd, EPOLL_CTL_DEL, pioobj->get_handle(), &evt);
	}

	{
		nm_utils::spin_scopelk_t lk(m_lkioobjcache);

	}
}

int32_t CEpollWrapper::exec()
{
	///wait io event...
	int32_t i32Ret = epoll_wait(m_i32epfd, m_tmpEvts, MAX_EVENTS, m_i32MSTimeout);
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
		pIoObj = static_cast<IIoObj*> (m_tmpEvts[i].data.ptr); ///no need add ref, for i have add it outside.
		ui32Evts = m_tmpEvts[i].events;

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

	///process bad io obj

	///

	return CMNERR_SUC;
}

}
