/*
 * sn_listener.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: rock
 */

#include "sn_listener_impl.h"
//#include "../smart_net/sn_endpoint_impl.h"


namespace nm_network
{

/**
 * CTcpListener
 * */

int32_t CTcpListener::open(const CIpv4Addr &listenaddr, int32_t i32backlog, const nm_engine::io_engine_ptr_t &pioengine)
{
	IF_TRUE_THEN_RETURN_CODE(NULL != m_pengine, CMNERR_COMMON_ERR);

	m_listenaddr = listenaddr;
	m_pengine = pioengine;
	m_i32backlog = i32backlog;

	///create listen socket.
	SYS_ASSERT(!m_tcpsock.is_opened());
	int32_t i32ret = m_tcpsock.open();
	IF_TRUE_THEN_RETURN_CODE(0 > i32ret, -1);

	i32ret = m_tcpsock.bind(listenaddr);
	IF_TRUE_THEN_RETURN_CODE(0 > i32ret, -2);

	i32ret = m_tcpsock.listen(i32backlog);
	IF_TRUE_THEN_RETURN_CODE(0 > i32ret, -3);

	i32ret = m_pengine->add_io_obj(io_obj_ptr_t(this));
	if (CMNERR_SUC > i32ret)
	{
		close();
	}

	return i32ret;
}

int32_t CTcpListener::add_endpoint(const endpoint_ptr_t &pEP)
{
	spin_scopelk_t lk(m_lktcpendpoints);

	std::pair<tcp_endpoint_map_t::iterator, bool> ret = m_maptcpendpoint.insert(dynamic_cast_smartptr<CTcpInboundEndpoint, IEndpoint>(pEP));

	return ret.second ? CMNERR_SUC : CMNERR_COMMON_ERR;
}

int32_t CTcpListener::del_endpoint(const endpoint_ptr_t &pEP)
{
	spin_scopelk_t lk(m_lktcpendpoints);

	return m_maptcpendpoint.erase(dynamic_cast_smartptr<CTcpInboundEndpoint, IEndpoint>(pEP)) > 0 ? CMNERR_SUC : CMNERR_COMMON_ERR;
}

int32_t CTcpListener::get_fd()
{
	return m_tcpsock.get_fd();
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
	using namespace nm_smartnet;

	///
	nm_network::sock_ptr_t psock = m_tcpsock.accept();
	if (NULL == psock)
	{
		SYS_ASSERT(false);
		return;
	}

	///find it.
	nm_framework::endpoint_ptr_t pep = NULL;
	{
		std::pair<tcp_endpoint_map_t::iterator, bool> ret =
				m_maptcpendpoint.find(psock->get_peer_addr());
		if (ret.second)
		{
pep		= *(ret.first)
	}
}

	{

	}

	pep->set_sock(psock);
	m_pengine->add_io_obj()
}


}
