/*
 * conn.cpp
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#include "sn_endpoint_impl.h"

namespace nm_smartnet
{
	/**
	 *
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
		return m_sm.post_event(EE_OPEN, &sp);
	}

	int32_t CTcpAcceptor::close()
	{
		return m_sm.post_event(EE_CLOSE, NULL);
	}

#define BACKLOG (20)
	int32_t CTcpAcceptor::opening(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		SParas *pPara = static_cast<SParas*> (pVoid);
		SYS_ASSERT(NULL == m_pTcpSock);
		m_pTcpSock = SYS_NOTRW_NEW(nm_network::CTcpSock);

		IF_TRUE_THEN_RETURN_CODE(m_pTcpSock->open() < 0, CMNERR_COMMON_ERR);
		IF_TRUE_THEN_RETURN_CODE(m_pTcpSock->bind(pPara->strIP, pPara->ui16Port) < 0, CMNERR_COMMON_ERR);
		IF_TRUE_THEN_RETURN_CODE(m_pTcpSock->listen(BACKLOG) < 0, CMNERR_COMMON_ERR);

		return CMNERR_SUC;
	}

	int32_t CTcpAcceptor::closing(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		SYS_ASSERT(m_pTcpSock->is_opened());
		SYS_ASSERT(CMNERR_SUC == m_pTcpSock->close());

		return CMNERR_SUC;
	}

	int32_t CTcpAcceptor::add_endpoint(const tcp_endpoint_ptr_t &pTcpEP)
	{
		if (ES_OPENNED == m_sm.get_cur_state()) ///weak check...
		{
			nm_utils::spin_scopelk_t lk(m_lkIdleEps);
			std::pair<tcp_endpoint_queue_t::iterator, bool> ret = m_queIdleEps.insert(pTcpEP);
			SYS_ASSERT(ret.second);
		}

		return CMNERR_SUC;
	}

	void CTcpAcceptor::handle_inserted_to_ioset(int32_t i32IoType, int32_t i32ReturnCode)
	{
		cmn_char_t szTmpBuf[256] = { 0 };
		TRACE_LOG(m_log, ELL_DEBUG, "acceptor (sock: %d, bind addr: %s, %hu) is inserted to ioset ,return: %d\n", get_fd(),
				m_bindAddr.get_ip_str(szTmpBuf, 255), m_bindAddr.get_port_hbo(), i32ReturnCode);

		SYS_ASSERT(i32ReturnCode >= 0 && EIT_INPUT_TYPE == i32IoType);
	}

	void CTcpAcceptor::handle_erased_from_ioset(int32_t i32IoType)
	{
		cmn_char_t szTmpBuf[256] = { 0 };
		TRACE_LOG(m_log, ELL_DEBUG, "acceptor (sock: %d, bind addr: %s, %hu) is erased from ioset\n", get_fd(), m_bindAddr.get_ip_str(szTmpBuf, 255),
				m_bindAddr.get_port_hbo());

		SYS_ASSERT(EIT_INPUT_TYPE == i32IoType);
	}

	void CTcpAcceptor::handle_input_evt()
	{
		///accept new connection
		nm_network::tcp_sock_ptr_t pNewSock = m_pTcpSock->accept();
		SYS_ASSERT(NULL != pNewSock); ///for test

		///
		tcp_endpoint_ptr_t pTcpEP = NULL;
		for (;;)
		{
			pTcpEP = NULL;

			{
				nm_utils::spin_scopelk_t lk(m_lkIdleEps);
				if (m_queIdleEps.empty())
				{
					break;
				}
				pTcpEP = m_queIdleEps.front();
				m_queIdleEps.pop();
			}

			if (CMNERR_SUC == pTcpEP->handle_connected(pNewSock))
			{
				break;
			}
		}

		if (NULL == pTcpEP)
		{
			pNewSock->close();
			pNewSock = NULL;
		}
	}

	void CTcpAcceptor::handle_output_evt()
	{
		SYS_ASSERT(false);
	}

	void CTcpAcceptor::handle_error_evt()
	{
		SYS_ASSERT(false);
	}

	/**
	 * tcp endpoint.
	 * */
	CTcpEndpoint::CTcpEndpoint(const tcp_acceptor_ptr_t &pTcpAcceptor) :
		m_sm(this), m_pTcpAcceptor(pTcpAcceptor), m_i32SMErrCode(-1)
	{
		m_sm.reg_evt_state(ES_CLOSED, EE_ADD, ES_ADDED, &CTcpEndpoint::handle_adding);

		m_sm.reg_evt_state(ES_ADDED, EE_CLOSE, ES_CLOSED, &CTcpEndpoint::handle_close_after_add);
		m_sm.reg_evt_state(ES_ADDED, EE_CONNECTED, ES_ADDING_OUTPUT_TASK, &CTcpEndpoint::handling_connected);

		m_sm.reg_evt_state(ES_OPENED_READY, EE_INTERNAL_ERR, ES_OPENED, &CTcpEndpoint::handle_internal_error);
		m_sm.reg_evt_state(ES_OPENED_READY, EE_OPENED, ES_OPENED, &CTcpEndpoint::handle_opened);

		m_sm.reg_evt_state(ES_OPENED, EE_INTERNAL_ERR, ES_CLOSING, &CTcpEndpoint::handle_closing);
		m_sm.reg_evt_state(ES_OPENED, EE_CLOSE, ES_CLOSING, &CTcpEndpoint::handle_closing);

		///not handling internal err, when in the next two state.
		m_sm.reg_evt_state(ES_CLOSING, EE_CLOSED, ES_CLOSED_READY, NULL);
		m_sm.reg_evt_state(ES_CLOSED_READY, EE_CLOSED, ES_CLOSED, &CTcpEndpoint::handle_closed);

		m_sm.set_cur_state(ES_CLOSED);
	}

	CTcpEndpoint::CTcpEndpoint(const tcp_connector_ptr_t &pTcpConnector) :
		m_sm(this), m_pTcpConnector(pTcpConnector), m_i32SMErrCode(-1)
	{
		m_sm.reg_evt_state(ES_CLOSED, EE_ADD, ES_ADDED, &CTcpEndpoint::handle_adding);

		m_sm.reg_evt_state(ES_ADDED, EE_INTERNAL_ERR, ES_OPENED_READY, &CTcpEndpoint::handle_internal_error);
		m_sm.reg_evt_state(ES_ADDED, EE_OPENED, ES_OPENED_READY, NULL);

		m_sm.reg_evt_state(ES_OPENED_READY, EE_INTERNAL_ERR, ES_OPENED, &CTcpEndpoint::handle_internal_error);
		m_sm.reg_evt_state(ES_OPENED_READY, EE_OPENED, ES_OPENED, &CTcpEndpoint::handle_opened);

		m_sm.reg_evt_state(ES_OPENED, EE_INTERNAL_ERR, ES_CLOSING, &CTcpEndpoint::handle_closing);
		m_sm.reg_evt_state(ES_OPENED, EE_CLOSE, ES_CLOSING, &CTcpEndpoint::handle_closing);

		///not handling internal err, when in the next two state.
		m_sm.reg_evt_state(ES_CLOSING, EE_CLOSED, ES_CLOSED_READY, NULL);
		m_sm.reg_evt_state(ES_CLOSED_READY, EE_CLOSED, ES_CLOSED, &CTcpEndpoint::handle_closed);

		m_sm.set_cur_state(ES_CLOSED);
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

		return m_sm.post_event(EE_ADD, NULL);
	}

	/**
	 * 此函数只将endpoint加入到connector或acceptor中
	 * */
	int32_t CTcpEndpoint::handle_adding(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		///init sm err code to zero, when opening.
		m_i32SMErrCode = CMNERR_SUC;

		int32_t i32Ret = 0;
		if (NULL != m_pTcpAcceptor)
		{
			SYS_ASSERT(NULL == m_pTcpConnector);
			i32Ret = m_pTcpAcceptor->add_endpoint(tcp_endpoint_ptr_t(this));
		}
		else
		{
			SYS_ASSERT(NULL != m_pTcpConnector);
			i32Ret = m_pTcpConnector->add_endpoint(tcp_endpoint_ptr_t(this));
		}

		return i32Ret;
	}

	int32_t CTcpEndpoint::handle_opened(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		///达到稳态了，可以处理在这个过程中出现的错误了
		if (CMNERR_SUC != m_i32SMErrCode)
		{
			TRACE_LOG(m_log, ELL_ERR, "errors occurred in the opening progress: %d\n", m_i32SMErrCode);
			close();
		}
		else
		{

		}

		TRACE_LOG(m_log, ELL_DEBUG, "openned!\n");

		on_openned();

	}

	int32_t CTcpEndpoint::handle_connected(nm_network::tcp_sock_ptr_t &pTcpSock)
	{
		return m_sm.post_event(EE_CONNECTED, &pTcpSock);
	}

	int32_t CTcpEndpoint::handle_closing(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		SYS_ASSERT(NULL != m_pTcpSock);
		m_pTcpSock->close();

		if (NULL != m_pTcpAcceptor)
		{
			m_pTcpAcceptor->del_endpoint(tcp_endpoint_ptr_t(this));
		}
		else
		{
			m_pTcpConnector->del_endpoint(tcp_endpoint_ptr_t(this));
		}

		return CMNERR_SUC;
	}

	int32_t CTcpEndpoint::handle_closed(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		///
	}

	void CTcpEndpoint::handle_inserted_to_ioset(int32_t i32IoType, int32_t i32ReturnCode)
	{
		SYS_ASSERT(CMNERR_SUC == i32ReturnCode);
		m_sm.post_event(EE_OPENED);
	}

	void CTcpEndpoint::handle_io_error(int32_t i32ErrCode)
	{
		m_sm.post_event(EE_IOERR, &i32ErrCode);
	}

	void CTcpEndpoint::handle_input_evt()
	{
		if (ES_OPENED == m_sm.get_cur_state()) ///有可能INPUT 处理线程先设置完毕，并开始接受数据，但是发送线程还在设置过程中，所以epoll要用level triger
		{
			on_receiving_data();
		}
	}

	void CTcpEndpoint::handle_output_evt()
	{
		if (ES_OPENED == m_sm.get_cur_state()) ///如果发生了错误，导致状态不是OPENNED时，就不用处理IO了。
		{
			on_sending_data();
		}
	}

	int32_t CTcpEndpoint::handling_io_err(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_pTcpSock->close();

		return NULL != m_pTcpAcceptor ? m_pTcpAcceptor->del_endpoint(tcp_endpoint_ptr_t(this)) : m_pTcpConnector->del_endpoint(
				tcp_endpoint_ptr_t(this));
	}

	///因为所有的对endpoint的状态进行修改的操作都是串行化的，所以这个时候EP一定是在其中的，删除一定是成功的。
	int32_t CTcpEndpoint::handle_close_after_add(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		return (NULL != m_pTcpAcceptor) ? m_pTcpAcceptor->del_endpoint(tcp_endpoint_ptr_t(this))
				: m_pTcpConnector->del_endpoint(tcp_endpoint_ptr_t(this));
	}

	int32_t CTcpEndpoint::handling_connected(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		return (NULL != m_pTcpAcceptor) ? m_pTcpAcceptor->add_endpoint()
	}

	int32_t CTcpEndpoint::handle_internal_error(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, cmn_pvoid_t pVoid)
	{
		m_i32SMErrCode = *(reinterpret_cast<int32_t*> (pvoid_t));

		return CMNERR_SUC;
	}

	/**
	 * close this endpoint.
	 * */
	int32_t CTcpEndpoint::close()
	{
		return m_sm.post_event(EE_CLOSE);

		//		IF_TRUE_THEN_RETURN_CODE(!m_bopenned, CMNERR_SUC);
		//
		//		nm_utils::mtx_scopelk_t lk(m_lkendpoint);
		//
		//		IF_TRUE_THEN_RETURN_CODE(!m_bopenned, CMNERR_SUC);
		//		m_bopenned = false;

		//		///first, delete it from engine.
		//		SYS_ASSERT(NULL != m_pSNEngine);
		//		int32_t i32ret = m_pSNEngine->del_endpoint(nm_framework::io_obj_ptr_t(this));
		//		SYS_ASSERT(CMNERR_SUC == i32ret);
		//		m_pSNEngine = NULL;
		//
		//		///then, close the socket.
		//		SYS_ASSERT(NULL != m_pSock);
		//		m_pSock->close();
		//		m_pSock = NULL;
		//
		//		return CMNERR_SUC;
	}

	int32_t CTcpEndpoint::get_type()
	{
		return E_TCP_INBOUND_ENDPOINT;
	}

/**
 * outbound
 * */
//CTcpOutboundEndpoint::CTcpOutboundEndpoint()
//{
//}
//
//CTcpOutboundEndpoint::~CTcpOutboundEndpoint()
//{
//}
//
//int32_t CTcpOutboundEndpoint::start(net_addr_ptr_t &pRemoteAddr,
//		int32_t i32ConnTimeout, int32_t i32MaxRetries)
//{
//	m_pConnecter = SYS_NOTRW_NEW(CTcpConnecter);
//}

}
