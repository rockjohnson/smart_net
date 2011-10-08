/*
 * sn_listener.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: rock
 */

#include "../network/sn_socket_impl.h"
#include "../smart_net/sn_endpoint_impl.h"
#include "sn_listener_impl.h"

#include "../engine/sn_engine.h"

namespace nm_network
{

/**
 * CTcpListener
 * */

int32_t CTcpListener::open(const CIpv4Addr &listenaddr, int32_t i32backlog,
		const nm_engine::engine_ptr_t &pengine)
{
	IF_TRUE_THEN_RETURN_CODE(NULL != m_pengine, CMNERR_COMMON_ERR);

	m_listenaddr = listenaddr;
	m_pengine = pengine;
	m_i32backlog = i32backlog;

	///create listen socket.
	SYS_ASSERT(!m_tcpsock.is_opened());
	int32_t i32ret = m_tcpsock.open();
	IF_TRUE_THEN_RETURN_CODE(0 > i32ret, -1);

	i32ret = m_tcpsock.bind(listenaddr);
	IF_TRUE_THEN_RETURN_CODE(0 > i32ret, -2);

	i32ret = m_tcpsock.listen(i32backlog);
	IF_TRUE_THEN_RETURN_CODE(0 > i32ret, -3);

	i32ret = m_pengine->add_io_obj(nm_engine::io_obj_ptr_t(this));
	if (CMNERR_SUC > i32ret)
	{
		close();
	}

	return i32ret;
}

int32_t CTcpListener::add_endpoint(const nm_framework::endpoint_ptr_t &pep)
{
	tcp_listener_evt_ptr_t ptlevt = SYS_NOTRW_NEW(CTcpListenerEvt(tcp_listener_ptr_t(this), pep, CTcpListenerEvt::EADD_EP));

	m_pengine->get_misc_task()->post_evt(ptlevt);

	return CMNERR_SUC; ///you see, you should control the endpoint state in endpoint. not here.
}

int32_t CTcpListener::del_endpoint(const nm_framework::endpoint_ptr_t &pep)
{
	tcp_listener_evt_ptr_t ptlevt = SYS_NOTRW_NEW(CTcpListenerEvt(tcp_listener_ptr_t(this), pep, CTcpListenerEvt::EDEL_EP));

	m_pengine->get_misc_task()->post_evt(ptlevt);

	return CMNERR_SUC;
}

int32_t CTcpListener::add_endpoint_ex(const nm_framework::endpoint_ptr_t &pep)
{
	tcp_ib_endpoint_ptr_t pibep = nm_utils::dynamic_cast_smartptr<
			nm_smartnet::CTcpInboundEndpoint, nm_framework::IEndpoint>(pep);

	if (pibep->get_peer_addr()
			== static_cast<nm_smartnet::CTcpInboundEndpoint*> (NULL))
	{
		nm_utils::spin_scopelk_t lk(m_lktcpendpoints);
		std::pair<tcp_ib_endpoint_set_t::iterator, bool> ret =
				m_settcpibendpoint.insert(pep);
		SYS_ASSERT(ret.second); ///must be true! you should control this in endpoint.

		return CMNERR_SUC;
	}

	std::pair<tcp_ib_endpoint_map_t::iterator, bool> ret =
			m_maptcpibendpoint.insert(dynamic_cast_smartptr<
					CTcpInboundEndpoint, IEndpoint> (pEP));

	return ret.second ? CMNERR_SUC : CMNERR_COMMON_ERR;
}

int32_t CTcpListener::del_endpoint_ex(const nm_framework::endpoint_ptr_t &pEP)
{
	spin_scopelk_t lk(m_lktcpendpoints);

	return m_maptcpibendpoint.erase(dynamic_cast_smartptr<CTcpInboundEndpoint,
			IEndpoint> (pEP)) > 0 ? CMNERR_SUC : CMNERR_COMMON_ERR;
}

int32_t CTcpListener::get_handle()
{
	return m_tcpsock.get_handle();
}

void CTcpListener::handle_output_evt()
{
	SYS_ASSERT(false);
}

void CTcpListener::handle_error_evt()
{
	SYS_ASSERT(false);
}

void CTcpListener::handle_input_evt()
{
	//	using namespace nm_smartnet;
	//
	//	///
	//	nm_network::sock_ptr_t psock = m_tcpsock.accept();
	//	if (NULL == psock)
	//	{
	//		SYS_ASSERT(false);
	//		return;
	//	}
	//
	//	///find it.
	//	nm_framework::endpoint_ptr_t pep = NULL;
	//	{
	//		std::pair<tcp_ib_endpoint_map_t::iterator, bool> ret =
	//				m_maptcpibendpoint.find(psock->get_peer_addr());
	//		if (ret.second)
	//		{
	//pep		= *(ret.first)
	//	}
	//}
	//
	//{
	//
	//}
	//
	//pep->set_sock(psock);
	//m_pengine->add_io_obj()
}

/**
 * tcp listener event.
 * */

CTcpListenerEvt::CTcpListenerEvt(tcp_listener_ptr_t &ptcplistener,
		nm_framework::endpoint_ptr_t &pep, int32_t i32opcode) :
	m_ptcplistener(ptcplistener), m_pendpoint(pep), m_i32opcode(i32opcode)
{
}

CTcpListenerEvt::~CTcpListenerEvt()
{
}

CTcpListenerEvt::handle()
{
	switch (m_i32opcode)
	{
	case EADD_EP:
	{
		m_ptcplistener->add_endpoint_ex(m_pendpoint);
		break;
	}
	case EDEL_EP:
	{
		m_ptcplistener->del_endpoint_ex(m_pendpoint);
		break;
	}
	default:
	{
		SYS_ASSERT(false);
		break;
	}
	}
}


}
