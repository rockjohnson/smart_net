/*
 * tcp_endpoint_tester.h
 *
 *  Created on: Oct 10, 2011
 *      Author: rock
 */

#ifndef TCP_ENDPOINT_TESTER_H_
#define TCP_ENDPOINT_TESTER_H_

#include <smart_net/smart_net.h>

namespace nm_busi
{

	class CTcpEndpointTester : public nm_smartnet::CTcpEndpoint
	{
	public:
		CTcpEndpointTester();
		virtual ~CTcpEndpointTester();
	};
	typedef nm_utils::CSmartPtr<nm_busi::CTcpEndpointTester> tcp_ep_tester_ptr_t;

}

#endif /* TCP_ENDPOINT_TESTER_H_ */
