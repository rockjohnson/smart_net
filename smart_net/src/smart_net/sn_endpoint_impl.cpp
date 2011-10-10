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
		m_sm.reg_evt_state(EES_CLOSED, EEE_OPEN, EES_OPENNED, &CTcpAcceptor::opening);
		m_sm.reg_evt_state(EES_OPENNED, EEE_CLOSE, EES_CLOSED, &CTcpAcceptor::closing);
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
		return m_sm.post_event(EEE_OPEN, &sp);
	}

	int32_t CTcpAcceptor::close()
	{
		return m_sm.post_event(EEE_CLOSE, NULL);
	}

#define BACKLOG (20)
	int32_t CTcpAcceptor::opening(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, pvoid_t pVoid)
	{
		SParas *pPara = static_cast<SParas*> (pVoid);
		SYS_ASSERT(!m_sock.is_opened());

		IF_TRUE_THEN_RETURN_CODE(m_sock.open() < 0, CMNERR_COMMON_ERR);
		IF_TRUE_THEN_RETURN_CODE(m_sock.bind(pPara->strIP, pPara->ui16Port) < 0, CMNERR_COMMON_ERR);
		IF_TRUE_THEN_RETURN_CODE(m_sock.listen(BACKLOG) < 0, CMNERR_COMMON_ERR);

		return CMNERR_SUC;
	}

	int32_t CTcpAcceptor::closing(int32_t i32CurState, int32_t i32Evt, int32_t i32NextState, pvoid_t pVoid)
	{
		SYS_ASSERT(m_sock.is_opened());
		SYS_ASSERT(CMNERR_SUC == m_sock.close());

		return CMNERR_SUC;
	}

	/**
	 * tcp endpoint.
	 * */
	CTcpEndpoint::CTcpEndpoint(const tcp_acceptor_ptr_t &pTcpAcceptor) :
		m_bopenned(false), m_sm(this)
	{
		///add into engine
		m_sm.reg_evt_state(ES_INIT, EE_ADD_INTO_ENGINE, ES_IN_ENGINE, NULL);

		///first add into recv thread.
		m_sm.reg_evt_state(ES_IN_ENGINE, EE_IN_ENGINE_ADD_INTO_RECV_THREAD, ES_IN_ENGINE_AND_IN_RECV_THREAD, NULL);
		m_sm.reg_evt_state(ES_IN_ENGINE_AND_IN_RECV_THREAD, EE_IN_ENGINE_ADD_INTO_SEND_THREAD, ES_IN_ENGINE_RUNNING, NULL);

		///first add into send thread.
		m_sm.reg_evt_state(ES_IN_ENGINE, EE_IN_ENGINE_ADD_INTO_SEND_THREAD, ES_IN_ENGINE_AND_IN_SEND_THREAD, NULL);
		m_sm.reg_evt_state(ES_IN_ENGINE_AND_IN_SEND_THREAD, EE_IN_ENGINE_ADD_INTO_RECV_THREAD, ES_IN_ENGINE_RUNNING, NULL);

		///

	}

	CTcpEndpoint::CTcpEndpoint(const tcp_connector_ptr_t &pTcpConnector) :
		m_bopenned(false), m_sm(this)
	{
		///add into engine
		m_sm.reg_evt_state(ES_INIT, EE_ADD_INTO_ENGINE, ES_IN_ENGINE, NULL);

		///first add into recv thread.
		m_sm.reg_evt_state(ES_IN_ENGINE, EE_IN_ENGINE_ADD_INTO_RECV_THREAD, ES_IN_ENGINE_AND_IN_RECV_THREAD, NULL);
		m_sm.reg_evt_state(ES_IN_ENGINE_AND_IN_RECV_THREAD, EE_IN_ENGINE_ADD_INTO_SEND_THREAD, ES_IN_ENGINE_RUNNING, NULL);

		///first add into send thread.
		m_sm.reg_evt_state(ES_IN_ENGINE, EE_IN_ENGINE_ADD_INTO_SEND_THREAD, ES_IN_ENGINE_AND_IN_SEND_THREAD, NULL);
		m_sm.reg_evt_state(ES_IN_ENGINE_AND_IN_SEND_THREAD, EE_IN_ENGINE_ADD_INTO_RECV_THREAD, ES_IN_ENGINE_RUNNING, NULL);

		///

	}

	CTcpEndpoint::~CTcpEndpoint()
	{
		// TODO Auto-generated destructor stub
	}

	/**
	 * open this endpoint and put it into the io engine.
	 * */
	int32_t CTcpEndpoint::open(const nm_network::CIpv4Addr &listenaddr, const nm_network::CIpv4Addr &peeraddr,
			const nm_framework::sn_engine_ptr_t psmartnetmgr)
	{
		IF_TRUE_THEN_RETURN_CODE(m_bopenned, CMNERR_SUC);

		nm_utils::mtx_scopelk_t lk(m_lkendpoint);

		IF_TRUE_THEN_RETURN_CODE(m_bopenned, CMNERR_SUC);
		m_bopenned = true;

		if (NULL != m_psmartnetmgr)
		{
			return CMNERR_COMMON_ERR;
		}
		m_psmartnetmgr = psmartnetmgr;

		m_psock = SYS_NOTRW_NEW(nm_network::CTcpSock);
		//		m_plistenaddr = listenaddr;
		//		m_ppeeraddr = peeraddr;

		return m_psmartnetmgr->add_endpoint(nm_framework::io_obj_ptr_t(this));
	}

	/**
	 * close this endpoint.
	 * */
	int32_t CTcpEndpoint::close()
	{
		IF_TRUE_THEN_RETURN_CODE(!m_bopenned, CMNERR_SUC);

		nm_utils::mtx_scopelk_t lk(m_lkendpoint);

		IF_TRUE_THEN_RETURN_CODE(!m_bopenned, CMNERR_SUC);
		m_bopenned = false;

		///first, delete it from engine.
		SYS_ASSERT(NULL != m_psmartnetmgr);
		int32_t i32ret = m_psmartnetmgr->del_endpoint(nm_framework::io_obj_ptr_t(this));
		SYS_ASSERT(CMNERR_SUC == i32ret);
		m_psmartnetmgr = NULL;

		///then, close the socket.
		SYS_ASSERT(NULL != m_psock);
		m_psock->close();
		m_psock = NULL;

		return CMNERR_SUC;
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
