/*
 * net_listener.cpp
 *
 *  Created on: Feb 12, 2011
 *      Author: rock
 */

#include "sn_listener.h"

namespace nm_smartnet
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

int32_t CTcpListener::init(INetAddr &bindAddr, int32_t i32Backlog)
{
	///create listen socket.
	int32_t i32Ret = m_tcpSock.create();
	IF_TRUE_THEN_RETURN_CODE(0 > i32Ret, -1);

	i32Ret = m_tcpSock.bind(bindAddr);
	IF_TRUE_THEN_RETURN_CODE(0 > i32Ret, -2);

	i32Ret = m_tcpSock.listen(i32Backlog);
	IF_TRUE_THEN_RETURN_CODE(0 > i32Ret, -3);

	return RET_SUC;
}

int32_t CTcpListener::get_fd()
{
	return m_tcpSock.get_fd();
}

}
