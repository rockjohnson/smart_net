/*
 * conn.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#include <sys/epoll.h>
#include "archive.h"
#include "sn_endpoint_impl.h"

namespace nm_smartnet
{
	namespace
	{
		/**
		 *
		 * */
		struct SParas
		{
			std::string strIP;
			u_int16_t ui16Port;
			u_int64_t ui64Val;
		};

		/**
		 *
		 * */
		struct SParasEx
		{
			cmn_string_t strMulticast;
			cmn_string_t strBindIp;
			u_int16_t ui16BindPort;
		};
	}

	using namespace nm_rup;
	/*--------------------------------------------------------------------------------------------------*/
	/**
	 *
	 * */
	CRupAcceptor::CRupAcceptor(const nm_framework::sn_engine_ptr_t &pSNEngine) :
		m_sm(this), m_pSNEngine(pSNEngine)
	{
		///原则：
		///1，只有在closed状态时，才能open，所以在任何其他状态，状态机都不提供open事件的处理；
		///2，close和internal err是可以在任何状态都要处理的事件，除了已经在关闭状态中或已经是closed时；
		m_sm.reg_evt_state(ES_CLOSED, EE_OPEN, ES_ADDING_INTO_IT, &CRupAcceptor::handling_closed_to_adding_into_it);

		m_sm.reg_evt_state(ES_ADDING_INTO_IT, EE_CLOSE, ES_ADDING_INTO_IT, &CRupAcceptor::handling_close_while_adding_into_it);
		m_sm.reg_evt_state(ES_ADDING_INTO_IT, EE_INTERNAL_ERR, ES_ADDING_INTO_IT, &CRupAcceptor::handling_internal_err_while_adding_into_it);
		m_sm.reg_evt_state(ES_ADDING_INTO_IT, EE_ADDED_INTO_IT, ES_OPENED, &CRupAcceptor::handling_adding_into_it_to_opened);

		m_sm.reg_evt_state(ES_OPENED, EE_CLOSE, ES_DELING_FROM_IT, &CRupAcceptor::handling_close_while_opened);
		m_sm.reg_evt_state(ES_OPENED, EE_INTERNAL_ERR, ES_DELING_FROM_IT, &CRupAcceptor::handling_internal_err_while_opened);

		m_sm.reg_evt_state(ES_DELING_FROM_IT, EE_DELED_FROM_IT, ES_CLOSED, &CRupAcceptor::handling_deling_from_it_to_closed);

		m_sm.set_cur_state(ES_CLOSED);
		///
		m_log.init(".", "tcp_acceptor_", ELL_DEBUG, 300);
		///
		set_io_evt(EIT_INPUT_TYPE, EPOLLIN);
	}

	/**
	 *
	 * */
	CRupAcceptor::~CRupAcceptor()
	{
	}

	/**
	 *
	 * */
	int32_t CRupAcceptor::open(const cmn_string_t &strIP, u_int16_t ui16Port)
	{
		SParas sp;
		sp.strIP = strIP;
		sp.ui16Port = ui16Port;
		return m_sm.post_evt(EE_OPEN, &sp);
	}

	void CRupAcceptor::on_opened()
	{
		TRACE_LOG(m_log, ELL_DEBUG, "on_opened\n");
	}

	void CRupAcceptor::on_closed()
	{
		TRACE_LOG(m_log, ELL_DEBUG, "on_closed\n");
	}

	int32_t CRupAcceptor::close()
	{
		return m_sm.post_evt(EE_CLOSE, NULL);
	}

#define BACKLOG (20)
	int32_t CRupAcceptor::handling_closed_to_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32PendingEvt = EE_NONE;
		SParas *pPara = static_cast<SParas*> (pVoid);
		CMN_ASSERT(NULL == m_pTcpSockListener);
		m_pTcpSockListener = SYS_NOTRW_NEW(nm_network::CRupSock);

		IF_TRUE_THEN_RETURN_CODE(m_pTcpSockListener->open() < 0, CMNERR_COMMON_ERR);
		IF_TRUE_THEN_RETURN_CODE(m_pTcpSockListener->bind(pPara->strIP, pPara->ui16Port) < 0, CMNERR_COMMON_ERR);
		IF_TRUE_THEN_RETURN_CODE(m_pTcpSockListener->listen(BACKLOG) < 0, CMNERR_COMMON_ERR);

		return m_pSNEngine->add_endpoint(tcp_acceptor_ptr_t(this), EIT_INPUT_TYPE);
	}

	int32_t CRupAcceptor::handling_close_while_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32PendingEvt = EE_CLOSE;

		return CMNERR_SUC;
	}

	int32_t CRupAcceptor::handling_close_while_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(m_pTcpSockListener->is_opened());
		m_pTcpSockListener->close();

		return m_pSNEngine->del_endpoint(tcp_acceptor_ptr_t(this), EIT_INPUT_TYPE);
	}

	int32_t CRupAcceptor::handling_internal_err_while_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		TRACE_LOG(m_log, ELL_DEBUG, "occured internal err\n");
		CMN_ASSERT(m_pTcpSockListener->is_opened());
		m_pTcpSockListener->close();

		return m_pSNEngine->del_endpoint(tcp_acceptor_ptr_t(this), EIT_INPUT_TYPE);
	}

	int32_t CRupAcceptor::handling_deling_from_it_to_closed(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		on_closed();

		return CMNERR_SUC;
	}

	int32_t CRupAcceptor::handling_internal_err_while_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		TRACE_LOG(m_log, ELL_DEBUG, "occured internal err\n");
		m_i32PendingEvt = EE_INTERNAL_ERR;

		return CMNERR_SUC;
	}

	int32_t CRupAcceptor::handling_adding_into_it_to_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
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

	int32_t CRupAcceptor::add_endpoint(const rup_endpoint_ptr_t &pTcpEp)
	{
		int32_t i32Ret = CMNERR_SUC;
		{
			nm_utils::spin_scopelk_t lk(m_lkIdleEndpoints);
			m_dqIdleEndpoints.push_back(pTcpEp);
		}

		return i32Ret;
	}

	int32_t CRupAcceptor::del_endpoint(const rup_endpoint_ptr_t &pTcpEp)
	{
		nm_utils::spin_scopelk_t lk(m_lkIdleEndpoints);

		bool bFlag = false;
		for (tcp_endpoint_deque_t::iterator iter = m_dqIdleEndpoints.begin(); iter != m_dqIdleEndpoints.end(); ++iter)
		{
			if ((*iter) == pTcpEp)
			{
				bFlag = true;
				m_dqIdleEndpoints.erase(iter);
				break;
			}
		}

		CMN_ASSERT(bFlag);

		return CMNERR_SUC;
	}

	void CRupAcceptor::handle_added_into_io_task(int32_t i32IoType, int32_t i32RetCode)
	{
		CMN_ASSERT(EIT_INPUT_TYPE == i32IoType);

		cmn_char_t szTmpBuf[256] = { 0 };
		TRACE_LOG(m_log, ELL_DEBUG, "acceptor (sock: %d, bind addr: %s, %hu) is inserted to ioset ,return: %d\n", get_ioobj_handle(), m_bindAddr.get_ip_str(szTmpBuf, 255), m_bindAddr.get_port_hbo(),
				i32RetCode);

		if (i32RetCode < CMNERR_SUC)
		{
			m_sm.post_evt(EE_INTERNAL_ERR, NULL);
		}

		(void) m_sm.post_evt(EE_ADDED_INTO_IT, &i32RetCode);
	}

	void CRupAcceptor::handle_deled_from_io_task(int32_t i32IoType)
	{
		cmn_char_t szTmpBuf[256] = { 0 };
		TRACE_LOG(m_log, ELL_DEBUG, "acceptor (sock: %d, bind addr: %s, %hu) is erased from ioset\n", get_ioobj_handle(), m_bindAddr.get_ip_str(szTmpBuf, 255), m_bindAddr.get_port_hbo());

		CMN_ASSERT(EIT_INPUT_TYPE == i32IoType);

		CMN_ASSERT(CMNERR_SUC == m_sm.post_evt(EE_DELED_FROM_IT, NULL));
	}

	/**
	 *
	 * */
	void CRupAcceptor::handle_input_evt()
	{
		///accept new connection
		nm_network::rup_sock_ptr_t pNewSock = m_pTcpSockListener->accept();
		CMN_ASSERT(NULL != pNewSock); ///for test

		///
		rup_endpoint_ptr_t pTcpEp = NULL;
		for (;;)
		{
			pTcpEp = NULL;

			{
				nm_utils::spin_scopelk_t lk(m_lkIdleEndpoints);
				if (m_dqIdleEndpoints.empty())
				{
					break;
				}
				pTcpEp = m_dqIdleEndpoints.front();
				m_dqIdleEndpoints.pop_front();
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

	void CRupAcceptor::handle_output_evt()
	{
		CMN_ASSERT(false);
	}

	void CRupAcceptor::handle_io_error(int32_t i32ErrCode)
	{
		CMN_ASSERT(CMNERR_SUC == m_sm.post_evt(EE_INTERNAL_ERR, NULL));
	}

	int32_t CRupAcceptor::get_ioobj_handle()
	{
		return m_pTcpSockListener->get_handle();
	}

	/*--------------------------------------------------------------------------------------------*/

	/**
	 *
	 * */
	CRupConnector::CRupConnector(const nm_framework::sn_engine_ptr_t &pSNEngine) :
		m_pSNEngine(pSNEngine), m_i32PendingEvt(EE_NONE), m_sm(this), m_ui64Interval(0)
	{
		m_sm.reg_evt_state(ES_CLOSED, EE_OPEN, ES_ADDING_INTO_TT, &CRupConnector::handling_closed_to_adding_into_tt);

		m_sm.reg_evt_state(ES_ADDING_INTO_TT, EE_CLOSE, ES_ADDING_INTO_TT, &CRupConnector::handling_close_while_adding_into_tt);
		m_sm.reg_evt_state(ES_ADDING_INTO_TT, EE_ADDED_INTO_TT, ES_CHECKING_TIMER, &CRupConnector::handling_adding_into_tt_to_checking_timer);

		m_sm.reg_evt_state(ES_CHECKING_TIMER, EE_CLOSE, ES_DELING_FROM_TT, &CRupConnector::handling_close_while_checking_timer);
		m_sm.reg_evt_state(ES_CHECKING_TIMER, EE_CONNECT, ES_ADDING_INTO_OT, &CRupConnector::handling_checking_timer_to_adding_into_ot);

		m_sm.reg_evt_state(ES_ADDING_INTO_OT, EE_CLOSE, ES_ADDING_INTO_OT, &CRupConnector::handling_close_while_adding_into_ot);
		m_sm.reg_evt_state(ES_ADDING_INTO_OT, EE_INTERNAL_ERR, ES_ADDING_INTO_OT, &CRupConnector::handling_internal_err_while_adding_into_ot);
		m_sm.reg_evt_state(ES_ADDING_INTO_OT, EE_ADDED_INTO_OT, ES_CONNECTING, &CRupConnector::handling_adding_into_ot_to_connecting);

		m_sm.reg_evt_state(ES_CONNECTING, EE_CLOSE, ES_DELING_FROM_OT_CLOSE, &CRupConnector::handling_close_while_connecting);
		m_sm.reg_evt_state(ES_CONNECTING, EE_INTERNAL_ERR, ES_DELING_FROM_OT_NORMAL, &CRupConnector::handling_internal_err_while_connecting);
		m_sm.reg_evt_state(ES_CONNECTING, EE_CONNECTED, ES_DELING_FROM_OT_NORMAL, &CRupConnector::handling_connected);

		m_sm.reg_evt_state(ES_DELING_FROM_OT_CLOSE, EE_DELED_FROM_OT, ES_DELING_FROM_TT, &CRupConnector::handling_deling_from_ot_close_to_deling_from_tt);

		m_sm.reg_evt_state(ES_DELING_FROM_OT_NORMAL, EE_CLOSE, ES_DELING_FROM_OT_NORMAL, &CRupConnector::handling_close_while_deling_from_ot_normal);
		m_sm.reg_evt_state(ES_DELING_FROM_OT_NORMAL, EE_INTERNAL_ERR, ES_DELING_FROM_OT_NORMAL, &CRupConnector::handling_internal_err_while_deling_from_ot_normal);
		m_sm.reg_evt_state(ES_DELING_FROM_OT_NORMAL, EE_DELED_FROM_OT, ES_CHECKING_TIMER, &CRupConnector::handling_deling_from_ot_normal_to_checking_timer);

		m_sm.reg_evt_state(ES_DELING_FROM_TT, EE_DELED_FROM_TT, ES_CLOSED, &CRupConnector::handling_deling_from_tt_to_closed);

		m_sm.set_cur_state(ES_CLOSED);
		///
		set_io_evt(EIT_OUTPUT_TYPE, EPOLLOUT);
		///
		m_log.init("tcp_connector", "c_", ELL_DEBUG, 60);
	}

	/**
	 *
	 * */
	CRupConnector::~CRupConnector()
	{
		{
			nm_utils::spin_scopelk_t lk(m_lkIdleEps);
			m_dqIdleEndpoints.clear();
		}

		m_pTcpSock = NULL;
	}

	/**
	 *
	 * */
	int32_t CRupConnector::open(const cmn_string_t &strAcceptorIP, u_int16_t ui16AcceptorPort, u_int64_t ui64Interval)
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
	int32_t CRupConnector::close()
	{
		return m_sm.post_evt(EE_CLOSE, NULL);
	}

	/**
	 *
	 * */
	void CRupConnector::handle_input_evt()
	{
		CMN_ASSERT(false);
	}

	/**
	 *
	 * */
	void CRupConnector::handle_output_evt()
	{
		(void) m_sm.post_evt(EE_CONNECTED, NULL);
	}

	void CRupConnector::handle_io_error(int32_t i32ErrCode)
	{
		(void) m_sm.post_evt(EE_INTERNAL_ERR, NULL);
	}

	void CRupConnector::handle_added_into_io_task(int32_t i32IoType, int32_t i32RetCode)
	{
		CMN_ASSERT(EIT_OUTPUT_TYPE == i32IoType);

		if (CMNERR_SUC != i32RetCode)
		{
			CMN_ASSERT(CMNERR_SUC == m_sm.post_evt(EE_INTERNAL_ERR, NULL));
		}
		else
		{
			(void) m_sm.post_evt(EE_ADDED_INTO_OT, NULL);
		}
	}

	void CRupConnector::handle_deled_from_io_task(int32_t i32IoType)
	{
		CMN_ASSERT(EIT_OUTPUT_TYPE == i32IoType);

		(void) m_sm.post_evt(EE_DELED_FROM_OT, NULL);
	}

	sock_handle_t CRupConnector::get_ioobj_handle()
	{
		CMN_ASSERT(NULL != m_pTcpSock);

		return m_pTcpSock->get_handle();
	}

	void CRupConnector::handle_add_into_timer_task()
	{
		m_sm.post_evt(EE_ADDED_INTO_TT, NULL);
	}

	void CRupConnector::handle_del_from_timer_task()
	{
		m_sm.post_evt(EE_DELED_FROM_TT, NULL);
	}

	/**
	 *
	 * */
	int32_t CRupConnector::add_endpoint(const rup_endpoint_ptr_t &pTcpEp)
	{
		nm_utils::spin_scopelk_t lk(m_lkIdleEps);
		m_dqIdleEndpoints.push_back(pTcpEp);

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CRupConnector::del_endpoint(const rup_endpoint_ptr_t &pTcpEp)
	{
		nm_utils::spin_scopelk_t lk(m_lkIdleEps);

		bool bFlag = false;
		for (tcp_endpoint_deque_t::iterator iter = m_dqIdleEndpoints.begin(); iter != m_dqIdleEndpoints.end(); ++iter)
		{
			if ((*iter) == pTcpEp)
			{
				bFlag = true;
				m_dqIdleEndpoints.erase(iter);
				break;
			}
		}

		CMN_ASSERT(bFlag);

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CRupConnector::handling_closed_to_adding_into_tt(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
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

	int32_t CRupConnector::handling_close_while_adding_into_tt(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32PendingEvt = EE_CLOSE;

		return CMNERR_SUC;
	}

	int32_t CRupConnector::handling_adding_into_tt_to_checking_timer(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
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
	int32_t CRupConnector::handling_close_while_checking_timer(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(NULL == m_pTcpSock);

		return m_pSNEngine->del_timer(tcp_connector_ptr_t(this));
	}

	/**
	 *
	 * */
	int32_t CRupConnector::handling_checking_timer_to_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(NULL == m_pTcpSock);
		m_pTcpSock = SYS_NOTRW_NEW(nm_network::CRupSock);
		m_pTcpSock->open();
		m_pTcpSock->set_nonblock(true);
		m_pTcpSock->connect(m_strAcceptorIp, m_ui16AcceptorPort);

		return m_pSNEngine->add_endpoint(tcp_connector_ptr_t(this), EIT_OUTPUT_TYPE);
	}

	/**
	 *
	 * */
	int32_t CRupConnector::handling_close_while_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32PendingEvt = EE_CLOSE;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CRupConnector::handling_adding_into_ot_to_connecting(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		///什么时候只赋值m_i32PendingEvt，什么时候判断m_i32PendingEvt需要解释下：
		///如果一个状态不是一个稳态的，如加入output task等，则只直接赋值m_i32PendingEvt，如果是一个稳态了，则开始处理m_i32PendingEvt。
		if (EE_NONE != m_i32PendingEvt)
		{
			m_pTcpSock->close();
			m_pSNEngine->del_endpoint(tcp_connector_ptr_t(this), EIT_OUTPUT_TYPE);
			m_sm.set_cur_state(EE_CLOSE == m_i32PendingEvt ? ES_DELING_FROM_OT_CLOSE : ES_DELING_FROM_OT_NORMAL);

			return CMNERR_COMMON_ERR;
		}

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CRupConnector::handling_close_while_connecting(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_pTcpSock->close();

		return m_pSNEngine->del_endpoint(tcp_connector_ptr_t(this), EIT_OUTPUT_TYPE);
	}

	int32_t CRupConnector::handling_internal_err_while_connecting(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		TRACE_LOG(m_log, ELL_DEBUG, "occured internal err\n");
		m_pTcpSock->close();

		return m_pSNEngine->del_endpoint(tcp_connector_ptr_t(this), EIT_OUTPUT_TYPE);
	}

	/**
	 *
	 * */
	int32_t CRupConnector::handling_connected(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(m_pTcpSock->is_opened());

		return m_pSNEngine->del_endpoint(tcp_connector_ptr_t(this), EIT_OUTPUT_TYPE);
	}

	/**
	 *
	 * */
	int32_t CRupConnector::handling_internal_err_while_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		TRACE_LOG(m_log, ELL_DEBUG, "occured internal err\n");
		m_i32PendingEvt = EE_INTERNAL_ERR;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CRupConnector::handling_deling_from_ot_close_to_deling_from_tt(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(!m_pTcpSock->is_opened());

		return m_pSNEngine->del_timer(tcp_connector_ptr_t(this));
	}

	/**
	 *
	 * */
	int32_t CRupConnector::handling_close_while_deling_from_ot_normal(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32PendingEvt = EE_CLOSE;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CRupConnector::handling_internal_err_while_deling_from_ot_normal(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		TRACE_LOG(m_log, ELL_DEBUG, "occured internal err\n");
		m_pTcpSock->close();

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CRupConnector::handling_deling_from_ot_normal_to_checking_timer(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		if (EE_NONE != m_i32PendingEvt)
		{
			CMN_ASSERT(EE_CLOSE == m_i32PendingEvt);

			m_pTcpSock->close();
			m_pTcpSock = NULL;
			m_pSNEngine->del_timer(tcp_connector_ptr_t(this));
			m_sm.set_cur_state(ES_DELING_FROM_TT);

			return CMNERR_COMMON_ERR;
		}

		if (!m_pTcpSock->is_opened()) ///ocurred internal err while deling from ot
		{
			m_pTcpSock = NULL;
			return CMNERR_SUC;
		}

		///established new connection
		CMN_ASSERT((NULL != m_pTcpSock) && m_pTcpSock->is_opened()); ///for test

		///
		rup_endpoint_ptr_t pTcpEp = NULL;
		for (;;)
		{
			pTcpEp = NULL;

			{
				nm_utils::spin_scopelk_t lk(m_lkIdleEps);
				if (m_dqIdleEndpoints.empty())
				{
					break;
				}
				pTcpEp = m_dqIdleEndpoints.front();
				m_dqIdleEndpoints.pop_front();
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
		}

		m_pTcpSock = NULL;

		return CMNERR_SUC;
	}

	int32_t CRupConnector::handling_deling_from_tt_to_closed(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		on_closed();

		return CMNERR_SUC;
	}

	void CRupConnector::on_opened()
	{
		TRACE_LOG(m_log, ELL_DEBUG, "tcp connect opened!\n");
	}

	void CRupConnector::on_closed()
	{
		TRACE_LOG(m_log, ELL_DEBUG, "tcp connect closed!\n");
	}

	/**
	 *
	 * */
	void CRupConnector::on_timer()
	{
		if (!m_dqIdleEndpoints.empty())
		{
			(void) m_sm.post_evt(EE_CONNECT, NULL);
		}
	}

	/*------------------------------------------------------------------------------------------------------*/
	/**
	 * tcp endpoint.
	 * */
	CRupEndpoint::CRupEndpoint(const tcp_acceptor_ptr_t &pTcpAcceptor) :
		m_sm(this), m_pTcpAcceptor(pTcpAcceptor), m_i32SMPendingEvt(EE_NONE), m_pSNEngine(pTcpAcceptor->get_engine())
	{
		init_sm();
		///
		set_io_evt(EIT_INPUT_TYPE, EPOLLIN);
		set_io_evt(EIT_OUTPUT_TYPE, EPOLLOUT);
		///
		m_log.init(".", "tcp_endpoint_", ELL_DEBUG, 60);
	}

	CRupEndpoint::CRupEndpoint(const tcp_connector_ptr_t &pTcpConnector) :
		m_sm(this), m_pTcpConnector(pTcpConnector), m_i32SMPendingEvt(-1), m_pSNEngine(pTcpConnector->get_engine())
	{
		init_sm();
		///
		set_io_evt(EIT_INPUT_TYPE, EPOLLIN);
		set_io_evt(EIT_OUTPUT_TYPE, EPOLLOUT);
		///
		char buf[1024] = { 0 };
		sprintf(buf, "%ld_", (long) this);
		m_log.init(".", buf, ELL_DEBUG, 60);
	}

	void CRupEndpoint::init_sm()
	{
		m_sm.reg_evt_state(ES_CLOSED, EE_OPEN, ES_ADDED_INTO_HELPER, &CRupEndpoint::handling_closed_to_added_into_helper);

		m_sm.reg_evt_state(ES_ADDED_INTO_HELPER, EE_CLOSE, ES_CLOSED, &CRupEndpoint::handling_added_into_helper_to_closed);
		m_sm.reg_evt_state(ES_ADDED_INTO_HELPER, EE_CONNECTED, ES_ADDING_INTO_OT, &CRupEndpoint::handling_added_into_helper_to_adding_into_ot);

		///这个时候发生关闭事件，则应该只设置个标志位，状态还是ES_ADDING_INTO_OUTPUT_TASK
		m_sm.reg_evt_state(ES_ADDING_INTO_OT, EE_CLOSE, ES_ADDING_INTO_OT, &CRupEndpoint::handling_close_while_adding_into_ot);
		m_sm.reg_evt_state(ES_ADDING_INTO_OT, EE_INTERNAL_ERR, ES_CLOSED, &CRupEndpoint::handling_internal_err_while_adding_into_ot);
		m_sm.reg_evt_state(ES_ADDING_INTO_OT, EE_ADDED_INTO_OT, ES_ADDING_INTO_IT, &CRupEndpoint::handling_added_into_ot_to_adding_into_it);

		///这个时候发生关闭or internal err事件，则应该只设置个标志位，状态还是ES_ADDING_INTO_INPUT_TASK
		m_sm.reg_evt_state(ES_ADDING_INTO_IT, EE_INTERNAL_ERR, ES_ADDING_INTO_IT, &CRupEndpoint::handling_internal_err_while_adding_into_it);
		m_sm.reg_evt_state(ES_ADDING_INTO_IT, EE_CLOSE, ES_ADDING_INTO_IT, &CRupEndpoint::handling_close_while_adding_into_it);
		m_sm.reg_evt_state(ES_ADDING_INTO_IT, EE_ADDED_INTO_IT, ES_OPENED, &CRupEndpoint::handling_adding_into_it_to_opened);

		///not handling internal err, when in the next two state.
		m_sm.reg_evt_state(ES_OPENED, EE_INTERNAL_ERR, ES_DELING_FROM_IT, &CRupEndpoint::handling_internal_err_while_opened);
		m_sm.reg_evt_state(ES_OPENED, EE_CLOSE, ES_DELING_FROM_IT, &CRupEndpoint::handling_close_while_opened);

		m_sm.reg_evt_state(ES_DELING_FROM_IT, EE_DELED_FROM_IT, ES_DELING_FROM_OT, &CRupEndpoint::handling_deling_from_it_to_deling_from_ot);
		m_sm.reg_evt_state(ES_DELING_FROM_OT, EE_DELED_FROM_OT, ES_CLOSED, &CRupEndpoint::handling_deling_from_ot_to_closed);

		m_sm.set_cur_state(ES_CLOSED);

	}

	CRupEndpoint::~CRupEndpoint()
	{
		// TODO Auto-generated destructor stub
	}

	/**
	 * open this endpoint and put it into the io engine.
	 * */
	int32_t CRupEndpoint::open()
	{
		return m_sm.post_evt(EE_OPEN, NULL);
	}

	sock_handle_t CRupEndpoint::get_ioobj_handle()
	{
		return m_pSock->get_handle();
	}

	/**
	 * 此函数只将endpoint加入到connector或acceptor中
	 * */
	int32_t CRupEndpoint::handling_closed_to_added_into_helper(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		///init sm err code to zero, when opening.
		m_i32SMPendingEvt = EE_NONE;

		m_ui64SendCnt = 0; ///the send count in a while
		m_ui64RecvCnt = 0; ///the recv count in a while
		m_ui64SendStore = 0;
		m_ui64RecvStore = 0;
		m_ui32CycleCnt = 0;

		return (NULL != m_pTcpAcceptor) ? m_pTcpAcceptor->add_endpoint(rup_endpoint_ptr_t(this)) : m_pTcpConnector->add_endpoint(rup_endpoint_ptr_t(this));
	}

	/**
	 *
	 * */
	void CRupEndpoint::handle_added_into_io_task(int32_t i32IoType, int32_t i32RetCode)
	{
		if (i32RetCode < 0)
		{
			m_sm.post_evt(EE_INTERNAL_ERR, NULL);
		}

		if (EIT_OUTPUT_TYPE == i32IoType)
		{
			m_sm.post_evt(EE_ADDED_INTO_OT, NULL);
		}
		else if (EIT_INPUT_TYPE == i32IoType)
		{
			m_sm.post_evt(EE_ADDED_INTO_IT, NULL);
		}
		else
		{
			CMN_ASSERT(false);
		}
	}

	/**
	 *
	 * */
	void CRupEndpoint::handle_deled_from_io_task(int32_t i32IoType)
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
	int32_t CRupEndpoint::handling_added_into_helper_to_closed(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(NULL == m_pSock);

		int32_t i32Ret = (NULL != m_pTcpAcceptor) ? m_pTcpAcceptor->del_endpoint(rup_endpoint_ptr_t(this)) : m_pTcpConnector->del_endpoint(rup_endpoint_ptr_t(this));
		CMN_ASSERT(CMNERR_SUC == i32Ret);

		///ugly code, but :)...
		m_sm.set_cur_state(ES_CLOSED);
		on_closed();
		m_pSock = NULL;
		return CMNERR_COMMON_ERR;

		//return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CRupEndpoint::handling_close_while_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		TRACE_LOG(m_log, ELL_DEBUG, "occurred close evt\n");
		m_i32SMPendingEvt = EE_CLOSE;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CRupEndpoint::handling_internal_err_while_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		TRACE_LOG(m_log, ELL_DEBUG, "occured internal err\n");
		m_i32SMPendingEvt = EE_INTERNAL_ERR;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CRupEndpoint::handling_internal_err_while_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		TRACE_LOG(m_log, ELL_DEBUG, "occured internal err\n");
		m_i32SMPendingEvt = EE_INTERNAL_ERR;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CRupEndpoint::handling_close_while_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		TRACE_LOG(m_log, ELL_DEBUG, "occurred close evt\n");
		m_i32SMPendingEvt = EE_CLOSE;

		return CMNERR_SUC;
	}

	int32_t CRupEndpoint::handling_internal_err_while_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		TRACE_LOG(m_log, ELL_DEBUG, "occured internal err\n");
		m_pSock->close();

		return m_pSNEngine->del_endpoint(rup_endpoint_ptr_t(this), EIT_INPUT_TYPE);
	}

	int32_t CRupEndpoint::handling_close_while_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		TRACE_LOG(m_log, ELL_DEBUG, "occurred close evt\n");
		m_pSock->close();

		return m_pSNEngine->del_endpoint(rup_endpoint_ptr_t(this), EIT_INPUT_TYPE);
	}

	int32_t CRupEndpoint::handling_deling_from_it_to_deling_from_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		return m_pSNEngine->del_endpoint(rup_endpoint_ptr_t(this), EIT_OUTPUT_TYPE);
	}

	int32_t CRupEndpoint::handling_deling_from_ot_to_closed(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_sm.set_cur_state(ES_CLOSED);
		on_closed();
		m_pSock = NULL;
		return CMNERR_COMMON_ERR;

		//return CMNERR_SUC;
	}

	int32_t CRupEndpoint::handling_added_into_helper_to_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		CMN_ASSERT(NULL == m_pSock);
		m_pSock = *(reinterpret_cast<nm_network::rup_sock_ptr_t*> (pVoid));

		return m_pSNEngine->add_endpoint(rup_endpoint_ptr_t(this), EIT_OUTPUT_TYPE);
	}

	int32_t CRupEndpoint::handle_connected(nm_network::rup_sock_ptr_t &pTcpSock)
	{
		return m_sm.post_evt(EE_CONNECTED, &pTcpSock);
	}

	/**
	 *
	 * */
	int32_t CRupEndpoint::handling_adding_into_it_to_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		if (EE_NONE != m_i32SMPendingEvt)
		{
			m_pSock->close();
			m_pSNEngine->del_endpoint(rup_endpoint_ptr_t(this), EIT_OUTPUT_TYPE);
			m_sm.set_cur_state(ES_DELING_FROM_OT);
			return CMNERR_COMMON_ERR;
		}

		///
		on_opened();

		return CMNERR_SUC;
	}

	void CRupEndpoint::handle_io_error(int32_t i32ErrCode)
	{
		m_sm.post_evt(EE_INTERNAL_ERR, &i32ErrCode);
	}

#define RECV_BUF (8192)
	void CRupEndpoint::handle_input_evt()
	{
		if (ES_OPENED != m_sm.get_cur_state()) ///有可能INPUT 处理线程先设置完毕，并开始接受数据，但是发送线程还在设置过程中，所以epoll要用level triger
		{
			return;
		}

		m_ui64RecvCnt++;
		int32_t i32Ret = m_pSock->handle_can_recv(RECV_BUF);
		if (CMNERR_SUC == i32Ret)
		{
			on_recved_data(m_pSock->get_recv_data());
		}
		else if (CMNERR_RECV_PENDING == i32Ret)
		{
			CMN_ASSERT(false);
		}
		else
		{
			TRACE_LAST_ERR(m_log, "handle_input_evt");
			m_sm.post_evt(EE_INTERNAL_ERR, NULL);
		}
	}

#define __MAX_CYCLE_CNT__ (1000000)
	void CRupEndpoint::handle_output_evt()
	{
		if (ES_OPENED == m_sm.get_cur_state()) ///如果发生了错误，导致状态不是OPENNED时，就不用处理IO了。
		{
			//TRACE_LOG(m_log, ELL_DEBUG, "handle_output_evt\n");
			if (m_pSock->handle_can_send() == CMNERR_IO_ERR)
			{
				m_sm.post_evt(EE_INTERNAL_ERR, NULL);
			}
			else
			{
				///keep alive
				if (++m_ui32CycleCnt > __MAX_CYCLE_CNT__)
				{
					m_ui32CycleCnt = 0;
					///need send hb?
					if (m_ui64SendCnt == m_ui64SendStore)
					{
						using namespace nm_pkg;
						///send hb
						CArchive<CPkgHdr, CPkgHB> ar;
						m_pSock->send(ar.serialize());
					}
					else
					{
						m_ui64SendStore = m_ui64SendCnt;
					}
					///peer is alive?
					if (m_ui64RecvCnt == m_ui64RecvStore)
					{
						///cut it down
						m_sm.post_evt(EE_INTERNAL_ERR, NULL);
					}
					else
					{
						m_ui64RecvStore = m_ui64RecvCnt;
					}
				}
			}
		}
	}

	/**
	 * handle business after inserted into output task, may failed.
	 * */
	int32_t CRupEndpoint::handling_added_into_ot_to_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		if (EE_CLOSE == m_i32SMPendingEvt)
		{
			m_pSock->close();
			m_pSNEngine->del_endpoint(rup_endpoint_ptr_t(this), EIT_OUTPUT_TYPE);
			m_sm.set_cur_state(ES_DELING_FROM_OT);
			return CMNERR_COMMON_ERR;
		}

		CMN_ASSERT(EE_NONE == m_i32SMPendingEvt);

		return m_pSNEngine->add_endpoint(rup_endpoint_ptr_t(this), EIT_INPUT_TYPE);
	}

	/**
	 * close this endpoint.
	 * */
	int32_t CRupEndpoint::close()
	{
		return m_sm.post_evt(EE_CLOSE, NULL);
	}

	/**
	 *
	 * */
	int32_t CRupEndpoint::send_data(nm_mem::mem_ptr_t &pData)
	{
		m_ui64SendCnt++;
		return m_sm.get_cur_state() == ES_OPENED ? m_pSock->send(pData) : CMNERR_COMMON_ERR;
	}

	/*------------------------------------------------------------------------------*/

#if 0
	using namespace nm_rmp;
	/**
	 *
	 * */
	CRmpEndpoint::CRmpEndpoint(nm_framework::sn_engine_ptr_t &pSnEngine, int32_t i32Type) :
		m_sm(this), m_pSNEngine(pSnEngine), m_i32Type(i32Type)
	{
		CMN_ASSERT(ERMP_SEND_ENDPOINT == i32Type || ERMP_RECV_ENDPOINT == i32Type);
		char buf[1024] = { 0 };
		sprintf(buf, "%p_", this);
		m_log.init("", buf, ELL_DEBUG, 60);
		init_sm();
	}

	/**
	 *
	 * */
	CRmpEndpoint::~CRmpEndpoint()
	{
	}

	/**
	 * init sm
	 * */
	int32_t CRmpEndpoint::init_sm()
	{
		//		CMN_ASSERT(CMNERR_SUC == m_sm.reg_evt_state(ES_ADDING_INTO_OT, EE_CLOSE, ES_ADDING_INTO_OT, &CRmpEndpoint::handling_close_while_adding_into_ot));
		//		CMN_ASSERT(CMNERR_SUC == m_sm.reg_evt_state(ES_ADDING_INTO_OT, EE_ADDED_INTO_OT, ES_ADDING_INTO_IT, &CRmpEndpoint::handling_added_into_ot_to_adding_into_it));
		//
		//		CMN_ASSERT(CMNERR_SUC == m_sm.reg_evt_state(ES_ADDING_INTO_IT, EE_INTERNAL_ERR, ES_ADDING_INTO_IT, &CRmpEndpoint::handling_added_into_ot_to_adding_into_it));
		//		CMN_ASSERT(CMNERR_SUC == m_sm.reg_evt_state(ES_ADDING_INTO_IT, EE_CLOSE, ES_ADDING_INTO_IT, &CRmpEndpoint::handling_added_into_ot_to_adding_into_it));
		//		CMN_ASSERT(CMNERR_SUC == m_sm.reg_evt_state(ES_ADDING_INTO_IT, EE_ADDED_INTO_IT, ES_OPENED, &CRmpEndpoint::handling_added_into_ot_to_adding_into_it));
		//
		//		CMN_ASSERT(CMNERR_SUC == m_sm.reg_evt_state(ES_OPENED, EE_CLOSE, ES_DELING_FROM_IT, &CRmpEndpoint::handling_added_into_ot_to_adding_into_it));
		//		CMN_ASSERT(CMNERR_SUC == m_sm.reg_evt_state(ES_OPENED, EE_INTERNAL_ERR, ES_DELING_FROM_IT, &CRmpEndpoint::handling_added_into_ot_to_adding_into_it));
		//
		//		CMN_ASSERT(CMNERR_SUC == m_sm.reg_evt_state(ES_DELING_FROM_IT, EE_DELED_FROM_IT, ES_DELING_FROM_OT, &CRmpEndpoint::handling_added_into_ot_to_adding_into_it));
		//		CMN_ASSERT(CMNERR_SUC == m_sm.reg_evt_state(ES_DELING_FROM_OT, EE_DELED_FROM_OT, ES_CLOSED, &CRmpEndpoint::handling_added_into_ot_to_adding_into_it));

		m_sm.reg_evt_state(ES_CLOSED, EE_OPEN, ES_ADDING_INTO_OT, &CRmpEndpoint::handling_closed_to_adding_into_ot);

		///这个时候发生关闭事件，则应该只设置个标志位，状态还是ES_ADDING_INTO_OUTPUT_TASK
		m_sm.reg_evt_state(ES_ADDING_INTO_OT, EE_CLOSE, ES_ADDING_INTO_OT, &CRmpEndpoint::handling_close_while_adding_into_ot);
		m_sm.reg_evt_state(ES_ADDING_INTO_OT, EE_INTERNAL_ERR, ES_CLOSED, &CRmpEndpoint::handling_internal_err_while_adding_into_ot);
		m_sm.reg_evt_state(ES_ADDING_INTO_OT, EE_ADDED_INTO_OT, ES_ADDING_INTO_IT, &CRmpEndpoint::handling_adding_into_ot_to_adding_into_it);

		///这个时候发生关闭or internal err事件，则应该只设置个标志位，状态还是ES_ADDING_INTO_INPUT_TASK
		m_sm.reg_evt_state(ES_ADDING_INTO_IT, EE_INTERNAL_ERR, ES_ADDING_INTO_IT, &CRmpEndpoint::handling_internal_err_while_adding_into_it);
		m_sm.reg_evt_state(ES_ADDING_INTO_IT, EE_CLOSE, ES_ADDING_INTO_IT, &CRmpEndpoint::handling_close_while_adding_into_it);
		m_sm.reg_evt_state(ES_ADDING_INTO_IT, EE_ADDED_INTO_IT, ES_OPENED, &CRmpEndpoint::handling_adding_into_it_to_opened);

		///not handling internal err, when in the next two state.
		m_sm.reg_evt_state(ES_OPENED, EE_INTERNAL_ERR, ES_DELING_FROM_IT, &CRmpEndpoint::handling_internal_err_while_opened);
		m_sm.reg_evt_state(ES_OPENED, EE_CLOSE, ES_DELING_FROM_IT, &CRmpEndpoint::handling_close_while_opened);

		m_sm.reg_evt_state(ES_DELING_FROM_IT, EE_DELED_FROM_IT, ES_DELING_FROM_OT, &CRmpEndpoint::handling_deling_from_it_to_deling_from_ot);
		m_sm.reg_evt_state(ES_DELING_FROM_OT, EE_DELED_FROM_OT, ES_CLOSED, &CRmpEndpoint::handling_deling_from_ot_to_closed);

		m_sm.set_cur_state(ES_CLOSED);
	}

	/**
	 *
	 * */
	int32_t CRmpEndpoint::open(const cmn_string_t &strMulticastIp, const cmn_string_t &strBindIp, u_int16_t ui16BindPort)
	{
		SParasEx sp;
		sp.strMulticast = strMulticastIp;
		sp.strBindIp = strBindIp;
		sp.ui16BindPort = ui16BindPort;

		return m_sm.post_evt(EE_OPEN, &sp);
	}

	/**
	 *
	 * */
	int32_t CRmpEndpoint::handling_closed_to_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		SParasEx *pSp = (SParasEx*) (pVoid);
		m_strMulticastIp = pSp->strMulticast;
		m_strBindIp = pSp->strBindIp;
		m_ui16BindPort = pSp->ui16BindPort;

		int32_t i32Ret = CMNERR_SUC;
		do
		{
			int32_t i32Type = ERMP_SEND_ENDPOINT == m_i32Type ? nm_network::RMP_SEND_SOCK : nm_network::RMP_RECV_SOCK;
			m_pSock = SYS_NOTRW_NEW(nm_network::CRmpSock(i32Type));
			i32Ret = m_pSock->open(m_strMulticastIp);
			if (CMNERR_SUC != i32Ret)
			{
				break;
			}
			i32Ret = m_pSock->set_nonblock(true);
		}
		while (false);

		if (CMNERR_SUC != i32Ret)
		{
			m_pSock = NULL;
			return CMNERR_COMMON_ERR;
		}

		return m_pSNEngine->add_endpoint(rmp_endpoint_ptr_t(this), EIT_OUTPUT_TYPE);
	}

	/**
	 *
	 * */
	int32_t CRmpEndpoint::handling_close_while_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		TRACE_LOG(m_log, ELL_DEBUG, "handle close evt\n");
		m_i32SMPendingEvt = EE_CLOSE;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	void CRmpEndpoint::handle_added_into_io_task(int32_t i32IoType, int32_t i32RetCode)
	{
		if (i32RetCode < 0)
		{
			m_sm.post_evt(EE_INTERNAL_ERR, NULL);
		}

		if (EIT_OUTPUT_TYPE == i32IoType)
		{
			m_sm.post_evt(EE_ADDED_INTO_OT, NULL);
		}
		else if (EIT_INPUT_TYPE == i32IoType)
		{
			m_sm.post_evt(EE_ADDED_INTO_IT, NULL);
		}
		else
		{
			CMN_ASSERT(false);
		}
	}

	/**
	 *
	 * */
	void CRmpEndpoint::handle_deled_from_io_task(int32_t i32IoType)
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
	int32_t CRmpEndpoint::handling_adding_into_ot_to_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		if (EE_CLOSE == m_i32SMPendingEvt)
		{
			m_pSock->close();
			m_pSNEngine->del_endpoint(rmp_endpoint_ptr_t(this), EIT_OUTPUT_TYPE);
			m_sm.set_cur_state(ES_DELING_FROM_OT);
			return CMNERR_COMMON_ERR;
		}

		CMN_ASSERT(EE_NONE == m_i32SMPendingEvt);

		return m_pSNEngine->add_endpoint(rmp_endpoint_ptr_t(this), EIT_INPUT_TYPE);
	}

	/**
	 *
	 * */
	int32_t CRmpEndpoint::handling_internal_err_while_adding_into_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		TRACE_LOG(m_log, ELL_DEBUG, "occured internal err\n");
		m_i32SMPendingEvt = EE_INTERNAL_ERR;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CRmpEndpoint::handling_internal_err_while_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		TRACE_LOG(m_log, ELL_DEBUG, "occured internal err\n");
		m_i32SMPendingEvt = EE_INTERNAL_ERR;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CRmpEndpoint::handling_close_while_adding_into_it(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		TRACE_LOG(m_log, ELL_DEBUG, "occurred close evt\n");
		m_i32SMPendingEvt = EE_CLOSE;

		return CMNERR_SUC;
	}

	/**
	 *
	 * */
	int32_t CRmpEndpoint::handling_internal_err_while_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		TRACE_LOG(m_log, ELL_DEBUG, "occurred internal err\n");
		m_pSock->close();

		return m_pSNEngine->del_endpoint(rmp_endpoint_ptr_t(this), EIT_INPUT_TYPE);
	}

	/**
	 *
	 * */
	int32_t CRmpEndpoint::handling_close_while_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		TRACE_LOG(m_log, ELL_DEBUG, "occurred close evt\n");
		m_pSock->close();

		return m_pSNEngine->del_endpoint(rmp_endpoint_ptr_t(this), EIT_INPUT_TYPE);
	}

	/**
	 *
	 * */
	int32_t CRmpEndpoint::handling_deling_from_it_to_deling_from_ot(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		return m_pSNEngine->del_endpoint(rmp_endpoint_ptr_t(this), EIT_OUTPUT_TYPE);
	}

	/**
	 *
	 * */
	int32_t CRmpEndpoint::handling_deling_from_ot_to_closed(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_sm.set_cur_state(ES_CLOSED);
		on_closed();
		m_pSock = NULL;
		return CMNERR_COMMON_ERR;
	}

	/**
	 *
	 * */
	int32_t CRmpEndpoint::handling_adding_into_it_to_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		if (EE_NONE != m_i32SMPendingEvt)
		{
			m_pSock->close();
			m_pSNEngine->del_endpoint(rmp_endpoint_ptr_t(this), EIT_OUTPUT_TYPE);
			m_sm.set_cur_state(ES_DELING_FROM_OT);
			return CMNERR_COMMON_ERR;
		}

		///
		on_opened();

		return CMNERR_SUC;
	}
#endif
}
