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

int32_t CSmartNet::start(u_int32_t ui32IoThreadCnt)
{
	m_pNetEngine = new CNetEngine;
}

int32_t CSmartNet::stop()
{

}

}
