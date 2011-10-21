/*
 * conn.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#include <sys/epoll.h>

#include "sn_endpoint_impl.h"

namespace nm_smartnet
{
	/*--------------------------------------------------------------------------------------------------*/
	/**
	 *
	 * */
	CTcpAcceptor::CTcpAcceptor(const nm_framework::sn_engine_ptr_t &pSNEngine) :
		m_sm(this), m_pSNEngine(pSNEngine)
	{
		m_sm.reg_evt_state(ES_CLOSED, EE_OPEN, ES_ADDING_INTO_IT,
				&CTcpAcceptor::handling_closed_to_adding_into_it);

		m_sm.reg_evt_state(ES_ADDING_INTO_IT, EE_CLOSE, ES_ADDING_INTO_IT,
				&CTcpAcceptor::handling_close_while_adding_into_it);
		m_sm.reg_evt_state(ES_ADDING_INTO_IT, EE_INTERNAL_ERR, ES_ADDING_INTO_IT,
				&CTcpAcceptor::handling_internal_err_while_adding_into_it);
		m_sm.reg_evt_state(ES_ADDING_INTO_IT, EE_ADDED_INTO_IT, ES_OPENED,
				&CTcpAcceptor::handling_adding_into_it_to_opened);

		m_sm.reg_evt_state(ES_OPENED, EE_CLOSE, ES_DELING_FROM_IT,
				&CTcpAcceptor::handling_opened_to_deling_from_it);
		m_sm.reg_evt_state(ES_DELING_FROM_IT, EE_DELED_FROM_IT, ES_CLOSED,
				&CTcpAcceptor::handling_deling_from_it_to_closed);
		m_sm.set_cur_state(ES_CLOSED);

		///
		m_log.init(".", "tcp_acceptor_", ELL_DEBUG, 300);
		///
		set_io_evt(EIT_INPUT_TYPE, EPOLLIN);
	}

	/**
	 *
	 * */
	CTcpAcceptor::~CTcpAcceptor()
	{
	}

	/**
	 *
	 * */
	struct SParas
	{
		std::string strIP;
		u_int16_t ui16Port;
		u_int64_t ui64Val;
	};

	int32_t CTcpAcceptor::open(const cmn_string_t &strIP, u_int16_t ui16Port)
	{
		SParas sp;
		sp.strIP = strIP;
		sp.ui16Port = ui16Port;
		return m_sm.post_evt(EE_OPEN, &sp);
	}

	void CTcpAcceptor::on_opened()
	{
		TRACE_LOG(m_log, ELL_DEBUG, "on_opened\n");
	}

	void CTcpAcceptor::on_closed()
	{
		TRACE_LOG(m_log, ELL_DEBUG, "on_closed\n");
	}

	int32_t CTcpAcceptor::close()
	{
		return m_sm.post_evt(EE_CLOSE, NULL);
	}

#define BACKLOG (20)
	int32_t CTcpAcceptor::handling_closed_to_adding_into_it(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32PendingEvt = EE_NONE;
		SParas *pPara = static_cast<SParas*> (pVoid);
		CMN_ASSERT(NULL == m_pTcpSockListener);
		m_pTcpSockListener = SYS_NOTRW_NEW(nm_network::CTcpSock);

		IF_TRUE_THEN_RETURN_CODE(m_pTcpSockListener->open() < 0, CMNERR_COMMON_ERR);
		IF_TRUE_THEN_RETURN_CODE(m_pTcpSockListener->bind(pPara->strIP, pPara->ui16Port) < 0,
				CMNERR_COMMON_ERR);
		IF_TRUE_THEN_RETURN_CODE(m_pTcpSockListener->listen(BACKLOG) < 0, CMNERR_COMMON_ERR);

		return m_pSNEngine->add_endpoint(tcp_acceptor_ptr_t(this), EIT_INPUT_TYPE);
	}

	int32_t CTcpAcceptor::handling_close_while_adding_into_it(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32PendingEvt = EE_CLOSE;

		return CMNERR_SUC;
	}

	int32_t CTcpAcceptor::handling_opened_to_deling_from_it(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(m_pTcpSockListener->is_opened());
		m_pTcpSockListener->close();

		return m_pSNEngine->del_endpoint(tcp_acceptor_ptr_t(this), EIT_INPUT_TYPE);
	}

	int32_t CTcpAcceptor::handling_deling_from_it_to_closed(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		on_closed();

		return CMNERR_SUC;
	}

	int32_t CTcpAcceptor::handling_internal_err_while_adding_into_it(int32_t i32CurState,
			int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32PendingEvt = EE_INTERNAL_ERR;

		return CMNERR_SUC;
	}

	int32_t CTcpAcceptor::handling_adding_into_it_to_opened(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		if (EE_NONE != m_i32PendingEvt)
		{
			m_pTcpSockListener->close();
			m_pSNEngine->del_endpoint(tcp_acceptor_ptr_t(this), EIT_INPUT_TYPE);

			m_sm.set_cur_state(ES_DELING_FROM_IT);

			return CMNERR_COMMON_ERR;
		}

		CMN_ASSERT(m_pTcpSockListener->is_opened());

		on_opened();

		return CMNERR_SUC;
	}

	int32_t CTcpAcceptor::add_endpoint(const tcp_endpoint_ptr_t &pTcpEp)
	{
		int32_t i32Ret = CMNERR_SUC;

		//if (ES_OPENED == m_sm.get_cur_state()) ///weak check...
		{
			nm_utils::spin_scopelk_t lk(m_lkIdleEps);
			m_dequeIdleEps.push_back(pTcpEp);
		}
//		else
//		{
//			i32Ret = CMNERR_COMMON_ERR;
//		}

		return i32Ret;
	}

	int32_t CTcpAcceptor::del_endpoint(const tcp_endpoint_ptr_t &pTcpEp)
	{
		nm_utils::spin_scopelk_t lk(m_lkIdleEps);

		bool bFlag = false;
		for (tcp_endpoint_deque_t::iterator iter = m_dequeIdleEps.begin(); iter
				!= m_dequeIdleEps.end(); ++iter)
		{
			if ((*iter) == pTcpEp)
			{
				bFlag = true;
				m_dequeIdleEps.erase(iter);
				break;
			}
		}

		CMN_ASSERT(bFlag);

		return CMNERR_SUC;
	}

	void CTcpAcceptor::handle_added_into_io_task(int32_t i32IoType, int32_t i32RetCode)
	{
		CMN_ASSERT(EIT_INPUT_TYPE == i32IoType);

		cmn_char_t szTmpBuf[256] = { 0 };
		TRACE_LOG(m_log, ELL_DEBUG,
				"acceptor (sock: %d, bind addr: %s, %hu) is inserted to ioset ,return: %d\n",
				get_ioobj_handle(), m_bindAddr.get_ip_str(szTmpBuf, 255),
				m_bindAddr.get_port_hbo(), i32RetCode);

		if (i32RetCode < CMNERR_SUC)
		{
			m_sm.post_evt(EE_INTERNAL_ERR, NULL);
		}

		m_sm.post_evt(EE_ADDED_INTO_IT, &i32RetCode);

	}

	void CTcpAcceptor::handle_deled_from_io_task(int32_t i32IoType)
	{
		cmn_char_t szTmpBuf[256] = { 0 };
		TRACE_LOG(m_log, ELL_DEBUG,
				"acceptor (sock: %d, bind addr: %s, %hu) is erased from ioset\n",
				get_ioobj_handle(), m_bindAddr.get_ip_str(szTmpBuf, 255), m_bindAddr.get_port_hbo());

		CMN_ASSERT(EIT_INPUT_TYPE == i32IoType);

		CMN_ASSERT(CMNERR_SUC == m_sm.post_evt(EE_DELED_FROM_IT, NULL));
	}

	void CTcpAcceptor::handle_input_evt()
	{
		///accept new connection
		nm_network::tcp_sock_ptr_t pNewSock = m_pTcpSockListener->accept();
		CMN_ASSERT(NULL != pNewSock); ///for test

		///
		tcp_endpoint_ptr_t pTcpEp = NULL;
		for (;;)
		{
			pTcpEp = NULL;

			{
				nm_utils::spin_scopelk_t lk(m_lkIdleEps);
				if (m_dequeIdleEps.empty())
				{
					break;
				}
				pTcpEp = m_dequeIdleEps.front();
				m_dequeIdleEps.pop_front();
			}

			if (CMNERR_SUC == pTcpEp->handle_connected(pNewSock))
			{
				break;
			}
#if 0
			else
			{
				///why error ? :), yes ,e.g. closing...
			}
#endif
		}

		if (NULL == pTcpEp)
		{
			pNewSock->close();
			pNewSock = NULL;
		}
	}

	void CTcpAcceptor::handle_output_evt()
	{
		CMN_ASSERT(false);
	}

	void CTcpAcceptor::handle_io_error(int32_t i32ErrCode)
	{
		CMN_ASSERT(false);
	}

	int32_t CTcpAcceptor::get_ioobj_handle()
	{
		return m_pTcpSockListener->get_handle();
	}

	/*------------------------------------------------------------------------------------------------------*/
	/**
	 * tcp endpoint.
	 * */
	CTcpEndpoint::CTcpEndpoint(const tcp_acceptor_ptr_t &pTcpAcceptor) :
		m_sm(this), m_pTcpAcceptor(pTcpAcceptor), m_i32SMPendingEvt(EE_NONE),
				m_pSNEngine(pTcpAcceptor->get_engine())
	{
		m_sm.reg_evt_state(ES_CLOSED, EE_OPEN, ES_ADDED_INTO_HELPER,
				&CTcpEndpoint::handling_closed_to_added_into_helper);

		m_sm.reg_evt_state(ES_ADDED_INTO_HELPER, EE_CLOSE, ES_CLOSED,
				&CTcpEndpoint::handling_added_into_helper_to_closed);
		m_sm.reg_evt_state(ES_ADDED_INTO_HELPER, EE_CONNECTED, ES_ADDING_INTO_OUTPUT_TASK,
				&CTcpEndpoint::handling_added_into_helper_to_adding_into_ot);

		///这个时候发生关闭事件，则应该只设置个标志位，状态还是ES_ADDING_INTO_OUTPUT_TASK
		m_sm.reg_evt_state(ES_ADDING_INTO_OUTPUT_TASK, EE_CLOSE, ES_ADDING_INTO_OUTPUT_TASK,
				&CTcpEndpoint::handling_close_while_adding_into_ot);
		m_sm.reg_evt_state(ES_ADDING_INTO_OUTPUT_TASK, EE_INTERNAL_ERR, ES_CLOSED,
				&CTcpEndpoint::handling_internal_err_while_adding_into_ot);
		m_sm.reg_evt_state(ES_ADDING_INTO_OUTPUT_TASK, EE_ADDED_INTO_OUTPUT_TASK,
				ES_ADDING_INTO_INPUT_TASK, &CTcpEndpoint::handling_added_into_ot_to_adding_into_it);

		///这个时候发生关闭or internal err事件，则应该只设置个标志位，状态还是ES_ADDING_INTO_INPUT_TASK
		m_sm.reg_evt_state(ES_ADDING_INTO_INPUT_TASK, EE_INTERNAL_ERR, ES_ADDING_INTO_INPUT_TASK,
				&CTcpEndpoint::handling_internal_err_while_adding_into_it);
		m_sm.reg_evt_state(ES_ADDING_INTO_INPUT_TASK, EE_CLOSE, ES_ADDING_INTO_INPUT_TASK,
				&CTcpEndpoint::handling_close_while_adding_into_it);
		m_sm.reg_evt_state(ES_ADDING_INTO_INPUT_TASK, EE_ADDED_INTO_INPUT_TASK, ES_OPENED,
				&CTcpEndpoint::handling_adding_into_it_to_opened);

		///not handling internal err, when in the next two state.
		m_sm.reg_evt_state(ES_OPENED, EE_INTERNAL_ERR, ES_DELING_FROM_IT,
				&CTcpEndpoint::handling_internal_err_while_opened);
		m_sm.reg_evt_state(ES_OPENED, EE_CLOSE, ES_DELING_FROM_IT,
				&CTcpEndpoint::handling_close_while_opened);

		m_sm.reg_evt_state(ES_DELING_FROM_IT, EE_DELED_FROM_IT, ES_DELING_FROM_OT,
				&CTcpEndpoint::handling_deling_from_it_to_deling_from_ot);
		m_sm.reg_evt_state(ES_DELING_FROM_OT, EE_DELED_FROM_OT, ES_CLOSED,
				&CTcpEndpoint::handling_deling_from_ot_to_closed);

		m_sm.set_cur_state(ES_CLOSED);

		///
		set_io_evt(EIT_INPUT_TYPE, EPOLLIN);
		set_io_evt(EIT_OUTPUT_TYPE, EPOLLOUT | EPOLLET);
	}

	CTcpEndpoint::CTcpEndpoint(const tcp_connector_ptr_t &pTcpConnector) :
		m_sm(this), m_pTcpConnector(pTcpConnector), m_i32SMPendingEvt(-1),
				m_pSNEngine(m_pTcpConnector->get_engine())
	{
		m_sm.reg_evt_state(ES_CLOSED, EE_OPEN, ES_ADDED_INTO_HELPER,
				&CTcpEndpoint::handling_closed_to_added_into_helper);

		m_sm.reg_evt_state(ES_ADDED_INTO_HELPER, EE_CLOSE, ES_CLOSED,
				&CTcpEndpoint::handling_added_into_helper_to_closed);
		m_sm.reg_evt_state(ES_ADDED_INTO_HELPER, EE_CONNECTED, ES_ADDING_INTO_OUTPUT_TASK,
				&CTcpEndpoint::handling_added_into_helper_to_adding_into_ot);

		///这个时候发生关闭事件，则应该只设置个标志位，状态还是ES_ADDING_INTO_OUTPUT_TASK
		m_sm.reg_evt_state(ES_ADDING_INTO_OUTPUT_TASK, EE_CLOSE, ES_ADDING_INTO_OUTPUT_TASK,
				&CTcpEndpoint::handling_close_while_adding_into_ot);
		m_sm.reg_evt_state(ES_ADDING_INTO_OUTPUT_TASK, EE_INTERNAL_ERR, ES_CLOSED,
				&CTcpEndpoint::handling_internal_err_while_adding_into_ot);
		m_sm.reg_evt_state(ES_ADDING_INTO_OUTPUT_TASK, EE_ADDED_INTO_OUTPUT_TASK,
				ES_ADDING_INTO_INPUT_TASK, &CTcpEndpoint::handling_added_into_ot_to_adding_into_it);

		///这个时候发生关闭or internal err事件，则应该只设置个标志位，状态还是ES_ADDING_INTO_INPUT_TASK
		m_sm.reg_evt_state(ES_ADDING_INTO_INPUT_TASK, EE_INTERNAL_ERR, ES_ADDING_INTO_INPUT_TASK,
				&CTcpEndpoint::handling_internal_err_while_adding_into_it);
		m_sm.reg_evt_state(ES_ADDING_INTO_INPUT_TASK, EE_CLOSE, ES_ADDING_INTO_INPUT_TASK,
				&CTcpEndpoint::handling_close_while_adding_into_it);
		m_sm.reg_evt_state(ES_ADDING_INTO_INPUT_TASK, EE_ADDED_INTO_INPUT_TASK, ES_OPENED,
				&CTcpEndpoint::handling_adding_into_it_to_opened);

		///not handling internal err, when in the next two state.
		m_sm.reg_evt_state(ES_OPENED, EE_INTERNAL_ERR, ES_DELING_FROM_IT,
				&CTcpEndpoint::handling_internal_err_while_opened);
		m_sm.reg_evt_state(ES_OPENED, EE_CLOSE, ES_DELING_FROM_IT,
				&CTcpEndpoint::handling_close_while_opened);

		m_sm.reg_evt_state(ES_DELING_FROM_IT, EE_DELED_FROM_IT, ES_DELING_FROM_OT,
				&CTcpEndpoint::handling_deling_from_it_to_deling_from_ot);
		m_sm.reg_evt_state(ES_DELING_FROM_OT, EE_DELED_FROM_OT, ES_CLOSED,
				&CTcpEndpoint::handling_deling_from_ot_to_closed);
		m_sm.set_cur_state(ES_CLOSED);

		///
		set_io_evt(EIT_INPUT_TYPE, EPOLLIN);
		set_io_evt(EIT_OUTPUT_TYPE, EPOLLOUT | EPOLLET);
	}

	CTcpEndpoint::~CTcpEndpoint()
	{
		// TODO Auto-generated destructor stub
	}

	/**
	 * open this endpoint and put it into the io engine.
	 * */
	int32_t CTcpEndpoint::open()
	{
		m_log.init(".", "tcp_endpoint_", ELL_DEBUG, 60);

		return m_sm.post_evt(EE_OPEN, NULL);
	}

	sock_handle_t CTcpEndpoint::get_ioobj_handle()
	{
		return m_pTcpSock->get_handle();
	}

	/**
	 * 此函数只将endpoint加入到connector或acceptor中
	 * */
	int32_t CTcpEndpoint::handling_closed_to_added_into_helper(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		///init sm err code to zero, when opening.
		m_i32SMPendingEvt = EE_NONE;

		return (NULL != m_pTcpAcceptor) ? m_pTcpAcceptor->add_endpoint(tcp_endpoint_ptr_t(this))
				: m_pTcpConnector->add_endpoint(tcp_endpoint_ptr_t(this));
	}

	/**
	 *
	 * */
	void CTcpEndpoint::handle_added_into_io_task(int32_t i32IoType, int32_t i32RetCode)
	{
		if (i32RetCode < 0)
		{
			m_sm.post_evt(EE_INTERNAL_ERR, NULL);
		}

		if (EIT_OUTPUT_TYPE == i32IoType)
		{
			m_sm.post_evt(EE_ADDED_INTO_OUTPUT_TASK, NULL);
		}
		else if (EIT_INPUT_TYPE == i32IoType)
		{
			m_sm.post_evt(EE_ADDED_INTO_INPUT_TASK, NULL);
		}
		else
		{
			CMN_ASSERT(false);
		}
	}

	/**
	 *
	 * */
	void CTcpEndpoint::handle_deled_from_io_task(int32_t i32IoType)
	{
		switch (i32IoType)
		{
			case EIT_INPUT_TYPE:
			{
				m_sm.post_evt(EE_DELED_FROM_IT, NULL);
				break;
			}
			case EIT_OUTPUT_TYPE:
			{
				m_sm.post_evt(EE_DELED_FROM_OT, NULL);
				break;
			}
			default:
			{
				CMN_ASSERT(false);
				break;
			}
		}
	}

	/**
	 *
	 * */
	int32_t CTcpEndpoint::handling_added_into_helper_to_closed(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(NULL == m_pTcpSock);

		int32_t i32Ret = (NULL != m_pTcpAcceptor) ? m_pTcpAcceptor->del_endpoint(
				tcp_endpoint_ptr_t(this)) : m_pTcpConnector->del_endpoint(tcp_endpoint_ptr_t(this));
		CMN_ASSERT(CMNERR_SUC == i32Ret);

		on_closed();

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CTcpEndpoint::handling_close_while_adding_into_ot(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32SMPendingEvt = EE_CLOSE;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CTcpEndpoint::handling_internal_err_while_adding_into_ot(int32_t i32CurState,
			int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(EE_NONE == m_i32SMPendingEvt);

		m_i32SMPendingEvt = EE_INTERNAL_ERR;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CTcpEndpoint::handling_internal_err_while_adding_into_it(int32_t i32CurState,
			int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32SMPendingEvt = EE_INTERNAL_ERR;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CTcpEndpoint::handling_close_while_adding_into_it(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32SMPendingEvt = EE_CLOSE;

		return CMNERR_SUC;
	}

	int32_t CTcpEndpoint::handling_internal_err_while_opened(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_pTcpSock->close();

		return m_pSNEngine->del_endpoint(tcp_endpoint_ptr_t(this), EIT_INPUT_TYPE);
	}

	int32_t CTcpEndpoint::handling_close_while_opened(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_pTcpSock->close();

		return m_pSNEngine->del_endpoint(tcp_endpoint_ptr_t(this), EIT_INPUT_TYPE);
	}

	int32_t CTcpEndpoint::handling_deling_from_it_to_deling_from_ot(int32_t i32CurState,
			int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		return m_pSNEngine->del_endpoint(tcp_endpoint_ptr_t(this), EIT_OUTPUT_TYPE);
	}

	int32_t CTcpEndpoint::handling_deling_from_ot_to_closed(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		on_closed();

		return CMNERR_SUC;
	}

	//	u_int32_t CTcpEndpoint::get_io_evt(int32_t i32IoType)
	//	{
	//		CMN_ASSERT(EIT_INPUT_TYPE == i32IoType || EIT_OUTPUT_TYPE == i32IoType);
	//
	//		return (EIT_INPUT_TYPE == i32IoType) ? EPOLLIN : EPOLLOUT;
	//	}

	int32_t CTcpEndpoint::handling_added_into_helper_to_adding_into_ot(int32_t i32CurState,
			int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(NULL == m_pTcpSock);
		m_pTcpSock = *(reinterpret_cast<nm_network::tcp_sock_ptr_t*> (pVoid));

		return m_pSNEngine->add_endpoint(tcp_endpoint_ptr_t(this), EIT_OUTPUT_TYPE);
	}

	int32_t CTcpEndpoint::handle_connected(nm_network::tcp_sock_ptr_t &pTcpSock)
	{
		return m_sm.post_evt(EE_CONNECTED, &pTcpSock);
	}

	/**
	 *
	 * */
	int32_t CTcpEndpoint::handling_adding_into_it_to_opened(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		if (EE_NONE != m_i32SMPendingEvt)
		{
			m_pTcpSock->close();
			m_pSNEngine->del_endpoint(tcp_endpoint_ptr_t(this), EIT_OUTPUT_TYPE);
			m_sm.set_cur_state(ES_DELING_FROM_OT);
			return CMNERR_COMMON_ERR;
		}

		(void) on_opened(CMNERR_SUC);

		return CMNERR_SUC;
	}

	void CTcpEndpoint::handle_io_error(int32_t i32ErrCode)
	{
		m_sm.post_evt(EE_INTERNAL_ERR, &i32ErrCode);
	}

	void CTcpEndpoint::handle_input_evt()
	{
		if (ES_OPENED == m_sm.get_cur_state()) ///有可能INPUT 处理线程先设置完毕，并开始接受数据，但是发送线程还在设置过程中，所以epoll要用level triger
		{
			TRACE_LOG(m_log, ELL_DEBUG, "handle_input_evt\n");
			nm_mem::mem_ptr_t pM;
			on_recved_data(pM);
		}
	}

	void CTcpEndpoint::handle_output_evt()
	{
		if (ES_OPENED == m_sm.get_cur_state()) ///如果发生了错误，导致状态不是OPENNED时，就不用处理IO了。
		{
			TRACE_LOG(m_log, ELL_DEBUG, "handle_output_evt\n");
		}
	}

	/**
	 * handle business after inserted into output task, may failed.
	 * */
	int32_t CTcpEndpoint::handling_added_into_ot_to_adding_into_it(int32_t i32CurState,
			int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		if (EE_CLOSE == m_i32SMPendingEvt)
		{
			m_pTcpSock->close();
			m_pSNEngine->del_endpoint(tcp_endpoint_ptr_t(this), EIT_OUTPUT_TYPE);
			m_sm.set_cur_state(ES_DELING_FROM_OT);
			return CMNERR_COMMON_ERR;
		}

		CMN_ASSERT(EE_NONE == m_i32SMPendingEvt);

		return m_pSNEngine->add_endpoint(tcp_endpoint_ptr_t(this), EIT_INPUT_TYPE);
	}

	/**
	 * close this endpoint.
	 * */
	int32_t CTcpEndpoint::close()
	{
		return m_sm.post_evt(EE_CLOSE, NULL);
	}

	/**
	 *
	 * */
	int32_t CTcpEndpoint::send_data(nm_mem::mem_ptr_t &pData)
	{
		return m_sm.get_cur_state() == ES_OPENED ? m_pTcpSock->send(pData) : CMNERR_COMMON_ERR;
	}

	/*--------------------------------------------------------------------------------------------*/

	/**
	 *
	 * */
	CTcpConnector::CTcpConnector(const nm_framework::sn_engine_ptr_t &pSNEngine) :
		m_pSNEngine(pSNEngine), m_i32PendingEvt(EE_NONE), m_sm(this), m_ui64Interval(0)
	{
		m_sm.reg_evt_state(ES_CLOSED, EE_OPEN, ES_ADDING_INTO_TT,
				&CTcpConnector::handling_closed_to_adding_into_tt);

		m_sm.reg_evt_state(ES_ADDING_INTO_TT, EE_CLOSE, ES_ADDING_INTO_TT,
				&CTcpConnector::handling_close_while_adding_into_tt);
		m_sm.reg_evt_state(ES_ADDING_INTO_TT, EE_ADDED_INTO_TT, ES_CHECKING_TIMER,
				&CTcpConnector::handling_adding_into_tt_to_checking_timer);

		m_sm.reg_evt_state(ES_CHECKING_TIMER, EE_CLOSE, ES_DELING_FROM_TT,
				&CTcpConnector::handling_close_while_checking_timer);
		m_sm.reg_evt_state(ES_CHECKING_TIMER, EE_CONNECT, ES_ADDING_INTO_OT,
				&CTcpConnector::handling_checking_timer_to_adding_into_ot);

		m_sm.reg_evt_state(ES_ADDING_INTO_OT, EE_CLOSE, ES_ADDING_INTO_OT,
				&CTcpConnector::handling_close_while_adding_into_ot);
		m_sm.reg_evt_state(ES_ADDING_INTO_OT, EE_INTERNAL_ERR, ES_ADDING_INTO_OT,
				&CTcpConnector::handling_internal_err_while_adding_into_ot);
		m_sm.reg_evt_state(ES_ADDING_INTO_OT, EE_ADDED_INTO_OT, ES_CONNECTING,
				&CTcpConnector::handling_adding_into_ot_to_connecting);

		m_sm.reg_evt_state(ES_CONNECTING, EE_CLOSE, ES_DELING_FROM_OT_CLOSE,
				&CTcpConnector::handling_close_while_connecting);
		m_sm.reg_evt_state(ES_CONNECTING, EE_INTERNAL_ERR, ES_DELING_FROM_OT_NORMAL,
				&CTcpConnector::handling_internal_err_while_connecting);
		m_sm.reg_evt_state(ES_CONNECTING, EE_CONNECTED, ES_DELING_FROM_OT_NORMAL,
				&CTcpConnector::handling_connected);

		m_sm.reg_evt_state(ES_DELING_FROM_OT_CLOSE, EE_DELED_FROM_OT, ES_DELING_FROM_TT,
				&CTcpConnector::handling_deling_from_ot_close_to_deling_from_tt);

		m_sm.reg_evt_state(ES_DELING_FROM_OT_NORMAL, EE_CLOSE, ES_DELING_FROM_OT_NORMAL,
				&CTcpConnector::handling_close_while_deling_from_ot_normal);
		m_sm.reg_evt_state(ES_DELING_FROM_OT_NORMAL, EE_DELED_FROM_OT, ES_CHECKING_TIMER,
				&CTcpConnector::handling_deling_from_ot_normal_to_checking_timer);

		m_sm.reg_evt_state(ES_DELING_FROM_TT, EE_DELED_FROM_TT, ES_CLOSED,
				&CTcpConnector::handling_deling_from_tt_to_closed);

		m_sm.set_cur_state(ES_CLOSED);

		///
		set_io_evt(EIT_OUTPUT_TYPE, EPOLLOUT);
	}

	/**
	 *
	 * */
	CTcpConnector::~CTcpConnector()
	{
		{
			nm_utils::spin_scopelk_t lk(m_lkIdleEps);
			m_dequeIdleEps.clear();
		}

		m_pTcpSock = NULL;
	}

	/**
	 *
	 * */
	int32_t CTcpConnector::open(const cmn_string_t &strAcceptorIP, u_int16_t ui16AcceptorPort,
			u_int64_t ui64Interval)
	{
		SParas sp;
		sp.strIP = strAcceptorIP;
		sp.ui16Port = ui16AcceptorPort;
		sp.ui64Val = ui64Interval;
		return m_sm.post_evt(EE_OPEN, &sp);
	}

	/**
	 *
	 * */
	int32_t CTcpConnector::close()
	{
		return m_sm.post_evt(EE_CLOSE, NULL);
	}

	/**
	 *
	 * */
	void CTcpConnector::handle_input_evt()
	{
		CMN_ASSERT(false);
	}

	/**
	 *
	 * */

	void CTcpConnector::handle_output_evt()
	{
		(void) m_sm.post_evt(EE_CONNECTED, NULL);
	}

	void CTcpConnector::handle_io_error(int32_t i32ErrCode)
	{
		(void) m_sm.post_evt(EE_INTERNAL_ERR, NULL);
	}

	void CTcpConnector::handle_added_into_io_task(int32_t i32IoType, int32_t i32RetCode)
	{
		CMN_ASSERT(EIT_OUTPUT_TYPE == i32IoType);

		if (CMNERR_SUC != i32RetCode)
		{
			CMN_ASSERT(CMNERR_SUC == m_sm.post_evt(EE_INTERNAL_ERR, NULL));
		}
		else
		{
			CMN_ASSERT(CMNERR_SUC == m_sm.post_evt(EE_ADDED_INTO_OT, NULL));
		}
	}

	void CTcpConnector::handle_deled_from_io_task(int32_t i32IoType)
	{
		CMN_ASSERT(EIT_OUTPUT_TYPE == i32IoType);

		CMN_ASSERT(CMNERR_SUC == m_sm.post_evt(EE_DELED_FROM_OT, NULL));
	}

	sock_handle_t CTcpConnector::get_ioobj_handle()
	{
		CMN_ASSERT(NULL != m_pTcpSock);
		return m_pTcpSock->get_handle();
	}

	void CTcpConnector::handle_add_into_timer_task()
	{
		m_sm.post_evt(EE_ADDED_INTO_TT, NULL);
	}

	void CTcpConnector::handle_del_from_timer_task()
	{
		m_sm.post_evt(EE_DELED_FROM_TT, NULL);
	}

	/**
	 *
	 * */
	int32_t CTcpConnector::add_endpoint(const tcp_endpoint_ptr_t &pTcpEp)
	{
		nm_utils::spin_scopelk_t lk(m_lkIdleEps);
		m_dequeIdleEps.push_back(pTcpEp);

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CTcpConnector::del_endpoint(const tcp_endpoint_ptr_t &pTcpEp)
	{
		nm_utils::spin_scopelk_t lk(m_lkIdleEps);

		bool bFlag = false;
		for (tcp_endpoint_deque_t::iterator iter = m_dequeIdleEps.begin(); iter
				!= m_dequeIdleEps.end(); ++iter)
		{
			if ((*iter) == pTcpEp)
			{
				bFlag = true;
				m_dequeIdleEps.erase(iter);
				break;
			}
		}

		CMN_ASSERT(bFlag);

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CTcpConnector::handling_closed_to_adding_into_tt(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32PendingEvt = EE_NONE;

		SParas *pSp = static_cast<SParas*> (pVoid);
		m_strAcceptorIp = pSp->strIP;
		m_ui16AcceptorPort = pSp->ui16Port;
		m_ui64Interval = pSp->ui64Val;

		CMN_ASSERT(m_ui64Interval > 0);

		set_interval(m_ui64Interval);

		///add into timer
		return m_pSNEngine->add_timer(tcp_connector_ptr_t(this));
	}

	int32_t CTcpConnector::handling_close_while_adding_into_tt(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32PendingEvt = EE_CLOSE;

		return CMNERR_SUC;
	}

	int32_t CTcpConnector::handling_adding_into_tt_to_checking_timer(int32_t i32CurState,
			int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		if (EE_NONE != m_i32PendingEvt)
		{
			CMN_ASSERT(EE_CLOSE == m_i32PendingEvt);
			m_pSNEngine->del_timer(tcp_connector_ptr_t(this));
			m_sm.set_cur_state(ES_DELING_FROM_TT);

			return CMNERR_COMMON_ERR;
		}

		on_opened();

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CTcpConnector::handling_close_while_checking_timer(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(NULL == m_pTcpSock);

		return m_pSNEngine->del_timer(tcp_connector_ptr_t(this));
	}

	/**
	 *
	 * */
	int32_t CTcpConnector::handling_checking_timer_to_adding_into_ot(int32_t i32CurState,
			int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(NULL == m_pTcpSock);
		m_pTcpSock = SYS_NOTRW_NEW(nm_network::CTcpSock);
		m_pTcpSock->set_nonblock(true);
		m_pTcpSock->connect(m_strAcceptorIp, m_ui16AcceptorPort);

		return m_pSNEngine->add_endpoint(tcp_connector_ptr_t(this), EIT_OUTPUT_TYPE);
	}

	/**
	 *
	 * */
	int32_t CTcpConnector::handling_close_while_adding_into_ot(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		//CMN_ASSERT(EE_NONE == m_i32PendingEvt);
		m_i32PendingEvt = EE_CLOSE;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CTcpConnector::handling_adding_into_ot_to_connecting(int32_t i32CurState,
			int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		///什么时候只赋值m_i32PendingEvt，什么时候判断m_i32PendingEvt需要解释下：
		///如果一个状态不是一个稳态的，如加入output task等，则只直接赋值m_i32PendingEvt，如果是一个稳态了，则开始处理m_i32PendingEvt。
		if (EE_NONE != m_i32PendingEvt)
		{
			m_pTcpSock->close();
			m_pSNEngine->del_endpoint(tcp_connector_ptr_t(this), EIT_OUTPUT_TYPE);
			m_sm.set_cur_state(
					EE_CLOSE == m_i32PendingEvt ? ES_DELING_FROM_OT_CLOSE
							: ES_DELING_FROM_OT_NORMAL);

			return CMNERR_COMMON_ERR;
		}
		//
		//		int32_t i32ErrCode = *(static_cast<int32_t*>(pVoid));
		//		if (CMNERR_SUC != i32ErrCode)
		//		{
		//			m_pTcpSock->close();
		//			m_sm.set_cur_state(ES_CHECKING_TIMER);
		//
		//			return CMNERR_COMMON_ERR;
		//		}

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CTcpConnector::handling_close_while_connecting(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_pTcpSock->close();

		return m_pSNEngine->del_endpoint(tcp_connector_ptr_t(this), EIT_OUTPUT_TYPE);
	}

	int32_t CTcpConnector::handling_internal_err_while_connecting(int32_t i32CurState,
			int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_pTcpSock->close();

		return m_pSNEngine->del_endpoint(tcp_connector_ptr_t(this), EIT_OUTPUT_TYPE);
	}

	/**
	 *
	 * */
	int32_t CTcpConnector::handling_connected(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(m_pTcpSock->is_opened());

		return m_pSNEngine->del_endpoint(tcp_connector_ptr_t(this), EIT_OUTPUT_TYPE);
	}

	/**
	 *
	 * */
	int32_t CTcpConnector::handling_internal_err_while_adding_into_ot(int32_t i32CurState,
			int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32PendingEvt = EE_INTERNAL_ERR;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CTcpConnector::handling_deling_from_ot_close_to_deling_from_tt(int32_t i32CurState,
			int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(!m_pTcpSock->is_opened());

		return m_pSNEngine->del_timer(tcp_connector_ptr_t(this));
	}

	/**
	 *
	 * */
	int32_t CTcpConnector::handling_close_while_deling_from_ot_normal(int32_t i32CurState,
			int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32PendingEvt = EE_CLOSE;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CTcpConnector::handling_deling_from_ot_normal_to_checking_timer(int32_t i32CurState,
			int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(m_pTcpSock->is_opened());

		if (EE_NONE != m_i32PendingEvt)
		{
			CMN_ASSERT(EE_CLOSE == m_i32PendingEvt);

			m_pTcpSock->close();
			m_pSNEngine->del_timer(tcp_connector_ptr_t(this));
			m_sm.set_cur_state(ES_DELING_FROM_TT);

			return CMNERR_COMMON_ERR;
		}

		///established new connection
		CMN_ASSERT((NULL != m_pTcpSock) && m_pTcpSock->is_opened()); ///for test

		///
		tcp_endpoint_ptr_t pTcpEp = NULL;
		for (;;)
		{
			pTcpEp = NULL;

			{
				nm_utils::spin_scopelk_t lk(m_lkIdleEps);
				if (m_dequeIdleEps.empty())
				{
					break;
				}
				pTcpEp = m_dequeIdleEps.front();
				m_dequeIdleEps.pop_front();
			}

			if (CMNERR_SUC == pTcpEp->handle_connected(m_pTcpSock))
			{
				break;
			}
#if 0
			else
			{
				///why error ? :), yes ,e.g. closing...
			}
#endif
		}

		if (NULL == pTcpEp)
		{
			m_pTcpSock->close();
			m_pTcpSock = NULL;
		}
	}

	int32_t CTcpConnector::handling_deling_from_tt_to_closed(int32_t i32CurState, int32_t i32Evt,
			int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		on_closed();

		return CMNERR_SUC;
	}

	void CTcpConnector::on_opened()
	{

	}

	void CTcpConnector::on_closed()
	{

	}

	/**
	 *
	 * */
	void CTcpConnector::on_timer()
	{
		if (!m_dequeIdleEps.empty())
		{
			m_sm.post_evt(EE_CONNECT, NULL);
		}
	}

}
