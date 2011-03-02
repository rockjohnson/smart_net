/*
 * smart_net.cpp
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 */

#include "smart_net.h"

namespace nm_smartnet
{

using namespace nm_framework;

CSmartNet::CSmartNet()
{
	// TODO Auto-generated constructor stub
}

CSmartNet::~CSmartNet()
{
	// TODO Auto-generated destructor stub
}

int32_t CSmartNet::start(u_int32_t ui32IoThreadCnt, int32_t i32IoEvtNotifier, int32_t i32MsTimeout)
{
	m_pNetEngine = new CNetEngine;
	IF_TRUE_THEN_RETURN_CODE(NULL == m_pNetEngine, CMNERR_COMMON_ERR);

	return m_pNetEngine->start(ui32IoThreadCnt, i32IoEvtNotifier, i32MsTimeout);
}

int32_t CSmartNet::stop()
{
	int32_t i32Ret = m_pNetEngine->stop();
	m_pNetEngine = NULL;

	return i32Ret;
}

io_engine_ptr_t& CSmartNet::get_io_engine()
{
	return m_pNetEngine;
}

}
