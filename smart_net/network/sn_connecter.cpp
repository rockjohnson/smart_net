/*
 * connector.cpp
 *
 *  Created on: Feb 12, 2011
 *      Author: rock
 */

#include "sn_connecter.h"

namespace nm_network
{

IConnecter::IConnecter()
{
	// TODO Auto-generated constructor stub

}

IConnecter::~IConnecter()
{
	// TODO Auto-generated destructor stub
}

/**
 * CTcpConnecter
 *
 * */
CTcpConnecter::CTcpConnecter()
{

}

CTcpConnecter::~CTcpConnecter()
{

}

int32_t CTcpConnecter::init()
{

}

int32_t CTcpConnecter::destroy()
{

}

/**
 * return value:
 * CMNERR_SUC : connect successfully immediately
 * SNERR_IN_PROGRESS : connect is in progress.
 * CMNERR_COMMON_ERR : error occurred.
 * */
int32_t CTcpConnecter::start()
{
	SYS_ASSERT(!m_sock.is_valid());

	///create socket
	int32_t i32Ret = m_sock.create();
	IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC != i32Ret, CMNERR_COMMON_ERR);

	i32Ret = m_sock.set_nonblock(true);
	IF_TRUE_THEN_RETURN_CODE(CMNERR_SUC != i32Ret, CMNERR_COMMON_ERR);

	i32Ret = m_sock.connect(m_remoteAddr);

	return i32Ret;
}

int32_t CTcpConnecter::stop()
{

}

void CTcpConnecter::handle_input_evt()
{
	SYS_ASSERT(false);
	EXIT_PROCESS(-1);
}

void CTcpConnecter::handle_output_evt()
{

}

void CTcpConnecter::handle_error_evt()
{
	SYS_ASSERT(false);
	EXIT_PROCESS(-1);
}

}
