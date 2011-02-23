/*
 * io_evt_notify.cpp
 *
 *  Created on: Dec 24, 2010
 *      Author: rock
 */

#include "sn_io_evt_notify.h"


namespace nm_framework
{

IIoEvtNotify::IIoEvtNotify()
{
	// TODO Auto-generated constructor stub
}

IIoEvtNotify::~IIoEvtNotify()
{
	destroy();
}

io_evt_notify_ptr_t IIoEvtNotify::create_obj(int32_t i32IoEvtNotifier)
{
	io_evt_notify_ptr_t pIoEvtNotify = NULL;

	if (EIEN_ALL <= i32IoEvtNotifier
			|| EIEN_NONE >= i32IoEvtNotifier)
	{
		return pIoEvtNotify;
	}

	switch (i32IoEvtNotifier)
	{
	case EIEN_SELECT:
	{
		pIoEvtNotify = SYS_NOTRW_NEW(CSelect);
		break;
	}
#if __PLATFORM__ == __PLATFORM_LINUX__
	case EIEN_EPOLL:
	{
		pIoEvtNotify = SYS_NOTRW_NEW(CEpoll);
		break;
	}
#endif
	default:
	{
		break;
	}
	}

	return pIoEvtNotify;
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
CEpoll::CEpoll()
:m_i32epfd(-1), m_i32MsTimeout(0)
{
	ZERO_MEM(&m_tmpEvts, sizeof(m_tmpEvts));
}

CEpoll::~CEpoll()
{
	// TODO Auto-generated destructor stub
}

#define UNUSED (10000) //Since Linux 2.6.8, the size argument is unused.  (The kernel dynamically sizes the required data structures without needing this initial hint.)
int32_t CEpoll::init(int32_t i32MsTimeout)
{
	SYS_ASSERT(-1 == m_i32epfd);

	m_i32MsTimeout = i32MsTimeout;

	return (m_i32epfd = epoll_create(UNUSED)) < 0 ? CMNERR_COMMON_ERR : CMNERR_SUC;
}

int32_t CEpoll::dispatch_evts()
{
	int32_t i32Ret = epoll_wait(m_i32epfd, m_tmpEvts, MAX_EVENTS, m_i32MsTimeout);
	if (-1 == i32Ret)
	{
		SYS_ASSERT(false);
		return CMNERR_FATAL_ERR;
	}

	IIoObj *pIoObj = NULL;
	u_int32_t ui32Evts = 0;
	for (int i = 0; i < i32Ret; i++)
	{
		pIoObj = static_cast<IIoObj*>(m_tmpEvts[i].data.ptr); ///no need add ref, for i have add it outside.
		ui32Evts = m_tmpEvts[i].events;

		//error
		if (ui32Evts & EPOLLERR)
		{
			pIoObj->handle_error_evt();
		}
		else
		{
			//input
			if (ui32Evts & EPOLLIN)
			{
				pIoObj->handle_input_evt();
			}

			//output
			if (ui32Evts & EPOLLOUT)
			{
				pIoObj->handle_output_evt();
			}
		}
	}

	return CMNERR_SUC;
}


}
