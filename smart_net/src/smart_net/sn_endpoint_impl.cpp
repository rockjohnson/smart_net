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
	/**
	 *
	 * */
	CTcpAcceptor::CTcpAcceptor(const nm_framework::sn_engine_ptr_t &pSNEngine) :
		m_sm(this), m_pSNEngine(pSNEngine), m_i32InputTaskId(0), m_i32OutputTaskId(0)
	{
		m_sm.reg_evt_state(ES_CLOSED, EE_OPEN, ES_OPENNED, &CTcpAcceptor::opening);
		m_sm.reg_evt_state(ES_OPENNED, EE_CLOSE, ES_CLOSED, &CTcpAcceptor::closing);
		m_log.init("./", "tcp_acceptor_", ELL_DEBUG, 300);
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
	};

	int32_t CTcpAcceptor::open(const cmn_string_t &strIP, u_int16_t ui16Port)
	{
		SParas sp;
		sp.strIP = strIP;
		sp.ui16Port = ui16Port;
		return m_sm.post_evt(EE_OPEN, &sp);
	}

	int32_t CTcpAcceptor::close()
	{
		return m_sm.post_evt(EE_CLOSE, NULL);
	}

#define BACKLOG (20)
	int32_t CTcpAcceptor::opening(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		SParas *pPara = static_cast<SParas*> (pVoid);
		CMN_ASSERT(NULL == m_pTcpSockListener);
		m_pTcpSockListener = SYS_NOTRW_NEW(nm_network::CTcpSock);

		IF_TRUE_THEN_RETURN_CODE(m_pTcpSockListener->open() < 0, CMNERR_COMMON_ERR);
		IF_TRUE_THEN_RETURN_CODE(m_pTcpSockListener->bind(pPara->strIP, pPara->ui16Port) < 0, CMNERR_COMMON_ERR);
		IF_TRUE_THEN_RETURN_CODE(m_pTcpSockListener->listen(BACKLOG) < 0, CMNERR_COMMON_ERR);

		return CMNERR_SUC;
	}

	int32_t CTcpAcceptor::closing(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(m_pTcpSockListener->is_opened());
		CMN_ASSERT(CMNERR_SUC == m_pTcpSockListener->close());

		return CMNERR_SUC;
	}

	int32_t CTcpAcceptor::add_endpoint(const tcp_endpoint_ptr_t &pTcpEp)
	{
		int32_t i32Ret = CMNERR_SUC;

		if (ES_OPENNED == m_sm.get_cur_state()) ///weak check...
		{
			nm_utils::spin_scopelk_t lk(m_lkIdleEps);
			m_dequeIdleEps.push_back(pTcpEp);
		}
		else
		{
			i32Ret = CMNERR_COMMON_ERR;
		}

		return i32Ret;
	}

	void CTcpAcceptor::handle_add_into_io_task(int32_t i32IoType, int32_t i32ReturnCode)
	{
		cmn_char_t szTmpBuf[256] = { 0 };
		TRACE_LOG(m_log, ELL_DEBUG, "acceptor (sock: %d, bind addr: %s, %hu) is inserted to ioset ,return: %d\n", get_fd(),
				m_bindAddr.get_ip_str(szTmpBuf, 255), m_bindAddr.get_port_hbo(), i32ReturnCode);

		CMN_ASSERT(i32ReturnCode >= 0 && EIT_INPUT_TYPE == i32IoType);
	}

	void CTcpAcceptor::handle_del_from_io_task(int32_t i32IoType)
	{
		cmn_char_t szTmpBuf[256] = { 0 };
		TRACE_LOG(m_log, ELL_DEBUG, "acceptor (sock: %d, bind addr: %s, %hu) is erased from ioset\n", get_fd(), m_bindAddr.get_ip_str(szTmpBuf, 255),
				m_bindAddr.get_port_hbo());

		CMN_ASSERT(EIT_INPUT_TYPE == i32IoType);
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

	void CTcpAcceptor::handle_io_error()
	{
		CMN_ASSERT(false);
	}

	int32_t CTcpAcceptor::get_fd()
	{
		return m_pTcpSockListener->get_handle();
	}

	/**
	 * tcp endpoint.
	 * */
	CTcpEndpoint::CTcpEndpoint(const tcp_acceptor_ptr_t &pTcpAcceptor) :
		m_sm(this), m_pTcpAcceptor(pTcpAcceptor), m_i32SMPendingEvt(EE_NONE), m_pSNEngine(m_pTcpAcceptor->get_engine())
	{
		m_sm.reg_evt_state(ES_CLOSED, EE_OPEN, ES_ADDED_INTO_HELPER, &CTcpEndpoint::handling_closed_to_added_into_helper);

		m_sm.reg_evt_state(ES_ADDED_INTO_HELPER, EE_CLOSE, ES_CLOSED, &CTcpEndpoint::handling_added_into_helper_to_closed);
		m_sm.reg_evt_state(ES_ADDED_INTO_HELPER, EE_CONNECTED, ES_ADDING_INTO_OUTPUT_TASK, &CTcpEndpoint::handling_added_into_helper_to_adding_into_ot);

		///这个时候发生关闭事件，则应该只设置个标志位，状态还是ES_ADDING_INTO_OUTPUT_TASK
		m_sm.reg_evt_state(ES_ADDING_INTO_OUTPUT_TASK, EE_CLOSE, ES_ADDING_INTO_OUTPUT_TASK, &CTcpEndpoint::handling_close_while_adding_into_ot);
		m_sm.reg_evt_state(ES_ADDING_INTO_OUTPUT_TASK, EE_INTERNAL_ERR, ES_CLOSED, &CTcpEndpoint::handling_internal_err_while_adding_into_ot);
		m_sm.reg_evt_state(ES_ADDING_INTO_OUTPUT_TASK, EE_ADDED_INTO_OUTPUT_TASK, ES_ADDING_INTO_INPUT_TASK, &CTcpEndpoint::handling_added_into_ot_to_adding_into_it);

		///这个时候发生关闭or internal err事件，则应该只设置个标志位，状态还是ES_ADDING_INTO_INPUT_TASK
		m_sm.reg_evt_state(ES_ADDING_INTO_INPUT_TASK, EE_INTERNAL_ERR, ES_ADDING_INTO_INPUT_TASK,
				&CTcpEndpoint::handling_internal_err_while_adding_into_it);
		m_sm.reg_evt_state(ES_ADDING_INTO_INPUT_TASK, EE_CLOSE, ES_ADDING_INTO_INPUT_TASK, &CTcpEndpoint::handling_close_while_adding_into_it);
		m_sm.reg_evt_state(ES_ADDING_INTO_INPUT_TASK, EE_ADDED_INTO_INPUT_TASK, ES_OPENED, &CTcpEndpoint::handling_adding_into_it_to_opened);

		///not handling internal err, when in the next two state.
		m_sm.reg_evt_state(ES_OPENED, EE_INTERNAL_ERR, ES_DELING_FROM_OT, &CTcpEndpoint::handling_internal_err_while_opened);
		m_sm.reg_evt_state(ES_OPENED, EE_CLOSE, ES_DELING_FROM_OT, &CTcpEndpoint::handling_close_while_opened);

		m_sm.reg_evt_state(ES_DELING_FROM_OT, EE_DELED_FROM_OT, ES_DELING_FROM_IT, &CTcpEndpoint::handling_deling_from_ot_to_deling_from_it);
		m_sm.reg_evt_state(ES_DELING_FROM_IT, EE_DELED_FROM_IT, ES_CLOSED, &CTcpEndpoint::handling_deling_from_it_to_closed);

		m_sm.set_cur_state(ES_CLOSED);
	}

	CTcpEndpoint::CTcpEndpoint(const tcp_connector_ptr_t &pTcpConnector) :
		m_sm(this), m_pTcpConnector(pTcpConnector), m_i32SMPendingEvt(-1), m_pSNEngine(m_pTcpConnector->get_engine())
	{
//		m_sm.reg_evt_state(ES_CLOSED, EE_OPEN, ES_ADDED_INTO_HELPER, &CTcpEndpoint::handle_adding);
//
//		m_sm.reg_evt_state(ES_ADDED_INTO_HELPER, EE_INTERNAL_ERR, ES_OPENED_READY, &CTcpEndpoint::handle_internal_error);
//		m_sm.reg_evt_state(ES_ADDED_INTO_HELPER, EE_OPENED, ES_OPENED_READY, NULL);
//
//		m_sm.reg_evt_state(ES_OPENED_READY, EE_INTERNAL_ERR, ES_OPENED, &CTcpEndpoint::handle_internal_error);
//		m_sm.reg_evt_state(ES_OPENED_READY, EE_OPENED, ES_OPENED, &CTcpEndpoint::handle_opened);
//
//		m_sm.reg_evt_state(ES_OPENED, EE_INTERNAL_ERR, ES_CLOSING, &CTcpEndpoint::handling_adding_into_it_to_opened);
//		m_sm.reg_evt_state(ES_OPENED, EE_CLOSE, ES_CLOSING, &CTcpEndpoint::handling_adding_into_it_to_opened);
//
//		///not handling internal err, when in the next two state.
//		m_sm.reg_evt_state(ES_CLOSING, EE_CLOSED, ES_CLOSED_READY, NULL);
//		m_sm.reg_evt_state(ES_CLOSED_READY, EE_CLOSED, ES_CLOSED, &CTcpEndpoint::handle_closed);
//
//		m_sm.set_cur_state(ES_CLOSED);
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
		m_log.init("./", "tcp_endpoint_", ELL_DEBUG, 60);

		return m_sm.post_evt(EE_OPEN, NULL);
	}

	/**
	 * 此函数只将endpoint加入到connector或acceptor中
	 * */
	int32_t CTcpEndpoint::handling_closed_to_added_into_helper(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		///init sm err code to zero, when opening.
		m_i32SMPendingEvt = EE_NONE;

		return (NULL != m_pTcpAcceptor) ? m_pTcpAcceptor->add_endpoint(tcp_endpoint_ptr_t(this)) : m_pTcpConnector->add_endpoint(
				tcp_endpoint_ptr_t(this));
	}

	/**
	 *
	 * */
	void CTcpEndpoint::handle_add_into_io_task(int32_t i32IoType, int32_t i32RetCode)
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
	int32_t CTcpEndpoint::handling_added_into_helper_to_closed(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(NULL == m_pTcpSock);

		int32_t i32Ret = (NULL != m_pTcpAcceptor) ? m_pTcpAcceptor->del_endpoint(tcp_endpoint_ptr_t(this)) : m_pTcpConnector->del_endpoint(
				tcp_endpoint_ptr_t(this));
		CMN_ASSERT(CMNERR_SUC == i32Ret);

		on_closed();

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CTcpEndpoint::handling_close_while_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32SMPendingEvt = EE_CLOSE;

		return CMNERR_SUC;
	}

	//	int32_t CTcpEndpoint::handling_internal_err_while_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	//	{
	//		m_pTcpSock->close();
	//		m_pTcpSock = NULL;
	//
	//		on_open(CMNERR_COMMON_ERR);
	//
	//		return CMNERR_SUC;
	//	}

	/**
	 *
	 * */
	int32_t CTcpEndpoint::handling_internal_err_while_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(EE_NONE == m_i32SMPendingEvt);

		m_i32SMPendingEvt = EE_INTERNAL_ERR;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CTcpEndpoint::handling_internal_err_while_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32SMPendingEvt = EE_INTERNAL_ERR;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CTcpEndpoint::handling_close_while_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32SMPendingEvt = EE_CLOSE;

		return CMNERR_SUC;
	}

	int32_t CTcpEndpoint::handling_internal_err_while_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_pTcpSock->close();

		return m_pSNEngine->del_endpoint(tcp_endpoint_ptr_t(this), EIT_OUTPUT_TYPE);
	}

	int32_t CTcpEndpoint::handling_close_while_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_pTcpSock->close();

		return m_pSNEngine->del_endpoint(tcp_endpoint_ptr_t(this), EIT_OUTPUT_TYPE);
	}

	int32_t CTcpEndpoint::handling_deling_from_ot_to_deling_from_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		return m_pSNEngine->del_endpoint(tcp_endpoint_ptr_t(this), EIT_INPUT_TYPE);
	}

	int32_t CTcpEndpoint::handling_deling_from_it_to_closed(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		on_closed();

		return CMNERR_SUC;
	}

	u_int32_t CTcpEndpoint::get_io_evt(int32_t i32IoType)
	{
		CMN_ASSERT(EIT_INPUT_TYPE == i32IoType || EIT_OUTPUT_TYPE == i32IoType);

		return (EIT_INPUT_TYPE == i32IoType) ? EPOLLIN : EPOLLOUT;
	}

	int32_t CTcpEndpoint::handling_added_into_helper_to_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(NULL == m_pTcpSock);
		m_pTcpSock = *(reinterpret_cast<nm_network::tcp_sock_ptr_t*> (pVoid));

		return m_pSNEngine->add_endpoint(tcp_endpoint_ptr_t(this), EIT_OUTPUT_TYPE);
	}

//	int32_t CTcpEndpoint::handle_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
//	{
//		///达到稳态了，可以处理在这个过程中出现的错误了
//		if (CMNERR_SUC != m_i32SMPendingEvt)
//		{
//			TRACE_LOG(m_log, ELL_ERR, "errors occurred in the opening progress: %d\n", m_i32SMPendingEvt);
//			close();
//		}
//		else
//		{
//
//		}
//
//		TRACE_LOG(m_log, ELL_DEBUG, "openned!\n");
//
//		on_openned();
//
//	}

	int32_t CTcpEndpoint::handle_connected(nm_network::tcp_sock_ptr_t &pTcpSock)
	{
		return m_sm.post_evt(EE_CONNECTED, &pTcpSock);
	}

	/**
	 *
	 * */
	int32_t CTcpEndpoint::handling_adding_into_it_to_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
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
		m_sm.post_evt(EE_IOERR, &i32ErrCode);
	}

	void CTcpEndpoint::handle_input_evt()
	{
		if (ES_OPENED == m_sm.get_cur_state()) ///有可能INPUT 处理线程先设置完毕，并开始接受数据，但是发送线程还在设置过程中，所以epoll要用level triger
		{
			//on_receiving_data();
		}
	}

	void CTcpEndpoint::handle_output_evt()
	{
		if (ES_OPENED == m_sm.get_cur_state()) ///如果发生了错误，导致状态不是OPENNED时，就不用处理IO了。
		{
			//on_sending_data();
		}
	}

//	int32_t CTcpEndpoint::handling_io_err(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
//	{
//		m_pTcpSock->close();
//
//		return NULL != m_pTcpAcceptor ? m_pTcpAcceptor->del_endpoint(tcp_endpoint_ptr_t(this)) : m_pTcpConnector->del_endpoint(
//				tcp_endpoint_ptr_t(this));
//	}

	///因为所有的对endpoint的状态进行修改的操作都是串行化的，所以这个时候EP一定是在其中的，删除一定是成功的。
//	int32_t CTcpEndpoint::handle_close_after_add(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
//	{
//		return (NULL != m_pTcpAcceptor) ? m_pTcpAcceptor->del_endpoint(tcp_endpoint_ptr_t(this)) : m_pTcpConnector->del_endpoint(
//				tcp_endpoint_ptr_t(this));
//	}
//
//	int32_t CTcpEndpoint::handling_connected(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
//	{
//		//return	(NULL != m_pTcpAcceptor) ? m_pTcpAcceptor->add_endpoint()
//	}

	/**
	 * handle business after inserted into output task, may failed.
	 * */
	int32_t CTcpEndpoint::handling_added_into_ot_to_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		if (EE_CLOSE == m_i32SMPendingEvt)
		{
			m_pTcpSock->close();
			m_pSNEngine->del_endpoint(tcp_endpoint_ptr_t(this), EIT_OUTPUT_TYPE);
			m_sm.set_cur_state(ES_CLOSED);
			return CMNERR_COMMON_ERR;
		}
		CMN_ASSERT(EE_NONE == m_i32SMPendingEvt);

		return m_pSNEngine->add_endpoint(tcp_endpoint_ptr_t(this), EIT_INPUT_TYPE);
	}

//	int32_t CTcpEndpoint::handle_internal_error(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
//	{
//		m_i32SMPendingEvt = *(reinterpret_cast<int32_t*> (pvoid_t));
//
//		return CMNERR_SUC;
//	}

	/**
	 * close this endpoint.
	 * */
	int32_t CTcpEndpoint::close()
	{
		return m_sm.post_evt(EE_CLOSE, NULL);
	}

//	int32_t CTcpEndpoint::get_type()
//	{
//		return E_TCP_INBOUND_ENDPOINT;
//	}

}
