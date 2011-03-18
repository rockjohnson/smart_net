/*
 * tcp_wrapper.cpp
 *
 *  Created on: Feb 24, 2011
 *      Author: rock
 */

#include "tcp_wrapper_impl.h"

#include <utils/smart_ptr.h>

#include "../smart_net/sn_endpoint_impl.h"
#include "../network/sn_listener_impl.h"

namespace nm_protocol
{

CTcpWrapper::CTcpWrapper()
{
}

CTcpWrapper::~CTcpWrapper()
{
}

int32_t CTcpWrapper::init(const nm_engine::engine_ptr_t &pengine)
{
}

int32_t CTcpWrapper::destroy()
{

}

int32_t CTcpWrapper::add_endpoint(const nm_framework::endpoint_ptr_t &pendpoint)
{
	using namespace nm_framework;
	using namespace nm_utils;
	using namespace nm_smartnet;
	using namespace nm_network;

	int32_t i32ret = CMNERR_SUC;
	switch (pendpoint->get_type())
	{
	case E_TCP_INBOUND_ENDPOINT:
	{
		tcp_ib_endpoint_ptr_t pIBEP = dynamic_cast_smartptr<CTcpInboundEndpoint, IEndpoint> (pendpoint);
		SYS_ASSERT(NULL == pIBEP);

		{
			mtx_scopelk_t lk(m_lk_tcp_listeners);
			tcp_listener_map_t::iterator iter = m_map_tcp_listeners.find(pIBEP->get_listen_addr());
			if (iter == m_map_tcp_listeners.end())
			{
				///create tcp listener, and listening.
				tcp_listener_ptr_t plistener = SYS_NOTRW_NEW(CTcpListener);
				i32ret = plistener->start(pIBEP->get_listen_addr(), m_pengine);
				if (CMNERR_SUC > i32ret)
				{
					break;
				}
				i32ret = plistener->add_endpoint(pendpoint);
				SYS_ASSERT(CMNERR_SUC <= i32ret);
				std::pair<tcp_listener_map_t::iterator, bool> ret =
						m_map_tcp_listeners.insert(
								tcp_listener_map_t::value_type(
										pIBEP->get_listen_addr(), plistener));
				SYS_ASSERT(ret.second);
//				i32ret = m_pengine->add_io_obj(plistener);
//				if (CMNERR_SUC > i32ret)
//				{
//					break;
//				}
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

int32_t CTcpWrapper::del_endpoint(const nm_framework::endpoint_ptr_t &pendpoint)
{

}

}
