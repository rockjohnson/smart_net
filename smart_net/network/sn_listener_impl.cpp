/*
 * sn_listener.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: rock
 */

#include "sn_listener_impl.h"

namespace nm_network
{

/**
 * CTcpListener
 * */

int32_t CTcpListener::open(const CIpv4Addr &listenaddr, int32_t i32backlog, const nm_engine::io_engine_ptr_t &pioengine)
{
	IF_TRUE_THEN_RETURN_CODE(NULL != m_pioengine, CMNERR_COMMON_ERR);

	m_listenaddr = listenaddr;
	m_pioengine = pioengine;
	m_i32backlog = i32backlog;

	///create listen socket.
	SYS_ASSERT(!m_tcpsock.is_opened());
	int32_t i32ret = m_tcpsock.open();
	IF_TRUE_THEN_RETURN_CODE(0 > i32ret, -1);

	i32ret = m_tcpsock.bind(listenaddr);
	IF_TRUE_THEN_RETURN_CODE(0 > i32ret, -2);

	i32ret = m_tcpsock.listen(i32backlog);
	IF_TRUE_THEN_RETURN_CODE(0 > i32ret, -3);

	i32ret = m_pioengine->add_io_obj(io_obj_ptr_t(this));
	if (CMNERR_SUC > i32ret)
	{
		close();
	}

	return i32ret;
}

int32_t CTcpListener::add_endpoint(const endpoint_ptr_t &pEP)
{
	spin_scopelk_t lk(m_lktcpendpoints);

	std::pair<tcp_endpoint_set_t::iterator, bool> ret = m_setTcpEndpoints.insert(dynamic_cast_smartptr<CTcpInboundEndpoint, IEndpoint>(pEP));

	return ret.second ? CMNERR_SUC : CMNERR_COMMON_ERR;
}

int32_t CTcpListener::del_endpoint(const endpoint_ptr_t &pEP)
{
	spin_scopelk_t lk(m_lktcpendpoints);

	return m_setTcpEndpoints.erase(dynamic_cast_smartptr<CTcpInboundEndpoint, IEndpoint>(pEP)) > 0 ? CMNERR_SUC : CMNERR_COMMON_ERR;
}

int32_t CTcpListener::get_fd()
{
	return m_tcpsock.get_fd();
}

}
