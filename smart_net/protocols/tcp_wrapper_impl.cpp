/*
 * tcp_wrapper.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: rock
 */

#include "tcp_wrapper_impl.h"
#include "../network/sn_endpoint_impl.h"

namespace nm_protocol
{

using namespace nm_framework;
using namespace nm_network;

int32_t CTcpWrapper::add_endpoint(const endpoint_ptr_t &pEP)
{
	int32_t i32ret = CMNERR_SUC;
	switch (pEP->get_type())
	{
	case E_TCP_INBOUND_ENDPOINT:
	{
		tcp_ib_endpoint_ptr_t pIBEP = dynamic_cast_smartptr<CTcpInboundEndpoint, IEndpoint>(pEP);
		SYS_ASSERT(NULL == pIBEP);

		{
			mtx_scopelk_t lk(m_lktcplisteners);
			tcp_listener_map_t::iterator iter = m_mapTcpListeners.find(pIBEP->get_listen_addr());
			if (iter == m_mapTcpListeners.end())
			{
				///create tcp listener, and listening.
				tcp_listener_ptr_t plistener = SYS_NOTRW_NEW(CTcpListener);
				i32ret = plistener->start(pIBEP->get_listen_addr());
				if (CMNERR_SUC > i32ret)
				{
					break;
				}
				i32ret = plistener->add_endpoint(pEP);
				SYS_ASSERT(CMNERR_SUC <= i32ret);
				std::pair<tcp_listener_map_t::iterator, bool> ret =
						m_mapTcpListeners.insert(
								tcp_listener_map_t::value_type(
										pEp->get_first_addr(), pEP));
				SYS_ASSERT(ret.second);
				i32ret = m_pIoEngine->add_io_obj(plistener);
				if (CMNERR_SUC > i32ret)
				{
					break;
				}
			}
			else
			{

			}
		}
		break;
	}
	case E_TCP_OUTBOUND_ENDPOINT:
	{
		break;
	}
	default:
	{
		break;
	}
	}

	return i32ret;
}





}
