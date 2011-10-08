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

	io_evt_notifier_ptr_t& CIoEvtNotifierFactory::create_obj(int32_t i32ioevtnotifier)
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
		m_i32epfd(-1), m_i32MsTimeout(0), m_i32IoType(0)
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
		SYS_ASSERT(-1 == m_i32epfd);
		SYS_ASSERT(m_vecIoObjsAddCache.empty());
		SYS_ASSERT(m_vecIoObjsDelCache.empty());
		SYS_ASSERT(m_setIoObjs.empty());

		m_i32IoType = i32IoTaskType;
		m_i32MsTimeout = i32MsTimeout;

		return (m_i32epfd = epoll_create(UNUSED)) < 0 ? CMNERR_COMMON_ERR : CMNERR_SUC;
	}

	///not thread safe
	int32_t CEpoll::destroy()
	{
		SYS_ASSERT(0 <= m_i32epfd);

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

	int32_t CEpoll::add_io_obj(const io_obj_ptr_t &pIoObj)
	{
		SYS_ASSERT(pIoObj != NULL);
		IF_TRUE_THEN_RETURN_CODE(0 == pIoObj->get_io_evt(m_i32IoType), CMNERR_SUC); ///no event

		{
			nm_utils::spin_scopelk_t lk(m_lkIoObjAddCache);
			std::pair<io_obj_vec_t::iterator, bool> pairRet = m_vecIoObjsAddCache.insert(pIoObj);
			SYS_ASSERT(pairRet.second);
		}

		return CMNERR_SUC;
	}

	int32_t CEpoll::del_io_obj(const io_obj_ptr_t &pIoObj)
	{
		SYS_ASSERT(pIoObj != NULL);
		IF_TRUE_THEN_RETURN_CODE(0 == pIoObj->get_io_evt(m_i32IoType), CMNERR_SUC); ///no event

		{
			nm_utils::spin_scopelk_t lk(m_lkIoObjDelCache);
			std::pair<io_obj_vec_t::iterator, bool> pairRet = m_vecIoObjsDelCache.insert(pIoObj);
			SYS_ASSERT(pairRet.second);
		}

		return CMNERR_SUC;
	}

	int32_t CEpoll::exec()
	{
		///del io obj from epoll set, the
		if (!m_vecIoObjsDelCache.empty())
		{
			io_obj_vec_t vecTmp(m_vecIoObjsDelCache.size());
			{
				nm_utils::spin_scopelk_t lk(m_lkIoObjDelCache);
				vecTmp.swap(m_vecIoObjsDelCache);
			}

			io_obj_set_t::size_type stRet = 0;
			for (io_obj_vec_t::iterator iter = vecTmp.begin(); iter != vecTmp.end(); ++iter)
			{
				if ((*iter)->get_fd() >= 0)
				{
					struct epoll_event evt;
					epoll_ctl(m_i32epfd, EPOLL_CTL_DEL, (*iter)->get_fd(), &evt);
				}

				stRet = m_setIoObjs.erase(*iter);
				SYS_ASSERT(1 == stRet);
				(*iter)->handle_erased_from_ioset(m_i32IoType);
			}

			vecTmp.clear();
		}

		///add io obj into epoll set
		if (!m_vecIoObjsAddCache.empty())
		{
			io_obj_vec_t vecTmp(m_vecIoObjsAddCache.size());
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

				i32Ret = epoll_ctl(m_i32epfd, EPOLL_CTL_ADD, (*iter)->get_fd(), &evt);
				if (0 == i32Ret)
				{
					pairRet = m_setIoObjs.insert(*iter);
					SYS_ASSERT(pairRet.second);
				}
				(*iter)->handle_inserted_to_ioset(m_i32IoType, i32Ret);
			}

			vecTmp.clear();
		}

		///wait io event...
		int32_t i32Ret = epoll_wait(m_i32epfd, m_arrEvts, MAX_EVENTS, m_i32MsTimeout);
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
