/*
 * sn_listener.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: rock
 */

#include "sn_listener.h"

namespace nm_network
{

IListener::IListener()
{
	// TODO Auto-generated constructor stub

}

IListener::~IListener()
{
	// TODO Auto-generated destructor stub
}

/**
 * CTcpListener
 * */

int32_t CTcpListener::start(net_addr_ptr_t &pBindAddr, int32_t i32Backlog, io_engine_ptr_t &pNetEngine)
{
	///create listen socket.
	int32_t i32Ret = m_tcpSock.create();
	IF_TRUE_THEN_RETURN_CODE(0 > i32Ret, -1);

	i32Ret = m_tcpSock.bind(*pBindAddr);
	IF_TRUE_THEN_RETURN_CODE(0 > i32Ret, -2);

	i32Ret = m_tcpSock.listen(i32Backlog);
	IF_TRUE_THEN_RETURN_CODE(0 > i32Ret, -3);

	m_pNetEngine = pNetEngine;
	IF_TRUE_THEN_RETURN_CODE(NULL == m_pNetEngine, -4);

	i32Ret = m_pNetEngine->add_io_obj(io_obj_ptr_t(this));
	if (CMNERR_SUC > i32Ret)
	{
		stop();
	}

	return i32Ret;
}

int32_t CTcpListener::get_fd()
{
	return m_tcpSock.get_fd();
}

}
