/*
 * rmp_endpoint_tester.h
 *
 *  Created on: Oct 10, 2011
 *      Author: rock
 */

#ifndef RMP_ENDPOINT_TESTER_H_
#define RMP_ENDPOINT_TESTER_H_

#include <smart_net/smart_net.h>

namespace nm_busi
{

	class CRmpEndpointTester : public nm_smartnet::nm_rmp::CRmpEndpoint
	{
	public:
		CRmpEndpointTester(nm_framework::sn_engine_ptr_t &pNetEngine, int32_t i32Type)
		:nm_smartnet::nm_rmp::CRmpEndpoint(pNetEngine, i32Type)
		{

		}
		virtual ~CRmpEndpointTester()
		{

		}

	public:
		virtual void on_opened()
		{

		}
		virtual void on_closed()
		{

		}
		virtual void on_recved_data(nm_mem::mem_ptr_t &pData)
		{

		}
	};
	typedef nm_utils::CSmartPtr<nm_busi::CRmpEndpointTester> rmp_ep_tester_ptr_t;
}

#endif /* RMP_ENDPOINT_TESTER_H_ */
