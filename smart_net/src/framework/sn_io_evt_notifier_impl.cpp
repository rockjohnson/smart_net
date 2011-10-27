/*
 * io_evt_notify.cpp
 *
 *  Created on: Dec 24, 2010
 *      Author: rock
 */

#include "sn_io_evt_notifier_impl.h"

namespace nm_event
{

	CIoEvtNotifierFactory::obj_ptr_t CIoEvtNotifierFactory::create_obj(int32_t i32IoEvtNotifier)
	{
		using namespace nm_framework;

		CIoEvtNotifierFactory::obj_ptr_t pIoEvtNotifier = NULL;

		if (EIEN_ALL <= i32IoEvtNotifier || EIEN_NONE >= i32IoEvtNotifier)
		{
			return pIoEvtNotifier;
		}

		switch (i32IoEvtNotifier)
		{
#if (0)
			case EIEN_SELECT:
			{
				pIoEvtNotifier = SYS_NOTRW_NEW(CSelect);
				break;
			}
#endif
#if __PLATFORM__ == __PLATFORM_LINUX__
			case EIEN_EPOLL:
			{
				pIoEvtNotifier = SYS_NOTRW_NEW(CEpoll);
				break;
			}
#endif
			default:
			{
				break;
			}
		}

		return pIoEvtNotifier;
	}

	//	/*
	//	 * select ..
	//	 * */
	//	CSelect::CSelect()
	//	{
	//		// TODO Auto-generated constructor stub
	//	}
	//
	//	CSelect::~CSelect()
	//	{
	//		// TODO Auto-generated destructor stub
	//	}


	/*----------------------------------------------------------------------------------------------*/
	/*
	 * epoll ...
	 * */
	CEpoll::CEpoll() :
		m_i32epfd(-1), m_i32IoType(0), m_i32MsTimeout(0)
	{
		ZERO_MEM(&m_arrEvts, sizeof(m_arrEvts));
	}

	CEpoll::~CEpoll()
	{
		// TODO Auto-generated destructor stub
		//destroy();
	}

#define UNUSED (10000) //Since Linux 2.6.8, the size argument is unused.  (The kernel dynamically sizes the required data structures without needing this initial hint.)
	int32_t CEpoll::init(int32_t i32IoTaskType, int32_t i32MsTimeout)
	{
		CMN_ASSERT(-1 == m_i32epfd);
		CMN_ASSERT(m_vecIoObjsAddCache.empty());
		CMN_ASSERT(m_vecIoObjsDelCache.empty());
		CMN_ASSERT(m_setIoObjs.empty());

		m_i32IoType = i32IoTaskType;
		m_i32MsTimeout = i32MsTimeout;

		char buf[1024] = { 0 };
		sprintf(buf, "./epoll_%d", i32IoTaskType);
		m_log.init(buf, "epoll_", ELL_DEBUG, 10);

		return (m_i32epfd = epoll_create(UNUSED)) < 0 ? CMNERR_COMMON_ERR : CMNERR_SUC;
	}

	///not thread safe
	int32_t CEpoll::destroy()
	{
		CMN_ASSERT(0 <= m_i32epfd);

		{
			nm_utils::spin_scopelk_t lk(m_lkIoObjAddCache);
			m_vecIoObjsAddCache.clear();
		}

		{
			nm_utils::spin_scopelk_t lk(m_lkIoObjDelCache);
			m_vecIoObjsDelCache.clear();
		}

		m_setIoObjs.clear(); ///not thread safe

		return CMNERR_COMMON_ERR;
	}

	int32_t CEpoll::add_ioobj(const nm_framework::io_obj_ptr_t &pIoObj)
	{
		CMN_ASSERT(NULL != pIoObj);
		IF_TRUE_THEN_RETURN_CODE(0 == pIoObj->get_io_evt(m_i32IoType), CMNERR_SUC); ///no event

		{
			nm_utils::spin_scopelk_t lk(m_lkIoObjAddCache);
			m_vecIoObjsAddCache.push_back(pIoObj);
		}

		return CMNERR_SUC;
	}

	int32_t CEpoll::del_ioobj(const nm_framework::io_obj_ptr_t &pIoObj)
	{
		CMN_ASSERT(NULL != pIoObj);
		IF_TRUE_THEN_RETURN_CODE(0 == pIoObj->get_io_evt(m_i32IoType), CMNERR_SUC); ///no event

		{
			nm_utils::spin_scopelk_t lk(m_lkIoObjDelCache);
			m_vecIoObjsDelCache.push_back(pIoObj);
		}

		return CMNERR_SUC;
	}

	int32_t CEpoll::exec()
	{
		///del io obj from epoll set, the
		if (!m_vecIoObjsDelCache.empty())
		{
			io_obj_vec_t vecTmp;
			{
				nm_utils::spin_scopelk_t lk(m_lkIoObjDelCache);
				vecTmp.swap(m_vecIoObjsDelCache);
			}

			io_obj_set_t::size_type stRet = 0;
			for (io_obj_vec_t::iterator iter = vecTmp.begin(); iter != vecTmp.end(); ++iter)
			{
				if ((*iter)->get_ioobj_handle() >= 0)
				{
					struct epoll_event evt;
					epoll_ctl(m_i32epfd, EPOLL_CTL_DEL, (*iter)->get_ioobj_handle(), &evt);
				}

				stRet = m_setIoObjs.erase(*iter);
				CMN_ASSERT(1 == stRet);
				(*iter)->handle_deled_from_io_task(m_i32IoType);
			}

			vecTmp.clear();
		}

		///add io obj into epoll set
		if (!m_vecIoObjsAddCache.empty())
		{
			io_obj_vec_t vecTmp;
			{
				nm_utils::spin_scopelk_t lk(m_lkIoObjAddCache);
				vecTmp.swap(m_vecIoObjsAddCache);
			}

			struct epoll_event evt = { 0 };
			int32_t i32Ret = 0;
			std::pair<io_obj_set_t::iterator, bool> pairRet;
			for (io_obj_vec_t::iterator iter = vecTmp.begin(); iter != vecTmp.end(); ++iter)
			{
				evt.events = (*iter)->get_io_evt(m_i32IoType);
				evt.data.ptr = (*iter).get_ptr();

				i32Ret = epoll_ctl(m_i32epfd, EPOLL_CTL_ADD, (*iter)->get_ioobj_handle(), &evt);
				if (0 == i32Ret)
				{
					pairRet = m_setIoObjs.insert(*iter);
					CMN_ASSERT(pairRet.second);
				}
				(*iter)->handle_added_into_io_task(m_i32IoType, i32Ret);
			}

			vecTmp.clear();
		}

		///wait io event...
		//TRACE_LOG(m_log, ELL_DEBUG, "epoll wait\n");
		int32_t i32Ret = epoll_wait(m_i32epfd, m_arrEvts, MAX_EVENTS, m_i32MsTimeout);
		if (-1 == i32Ret)
		{
			if (EINTR == errno)
			{
				return CMNERR_SUC;
			}

			TRACE_LAST_ERR(m_log, "epoll_wait error!\n");
			CMN_ASSERT(false);
			return CMNERR_FATAL_ERR;
		}

		///process io evt...
		nm_framework::IIoObj *pIoObj = NULL;
		u_int32_t ui32Evts = 0;
		for (int i = 0; i < i32Ret; i++)
		{
			pIoObj = static_cast<nm_framework::IIoObj*> (m_arrEvts[i].data.ptr); ///no need add ref, for i have add it outside.
			ui32Evts = m_arrEvts[i].events;

			//error
			if ((ui32Evts & EPOLLERR) || (ui32Evts & EPOLLHUP))
			{
				//CMN_ASSERT(false); ///check how this happened
				pIoObj->handle_io_error(CMNERR_IO_ERR);
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
					CMN_ASSERT(false);
				}
			}
		}

		return CMNERR_SUC;
	}

}
