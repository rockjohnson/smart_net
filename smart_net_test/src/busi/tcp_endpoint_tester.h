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

	class CTcpEndpointTester : public nm_smartnet::nm_rup::CRupEndpoint
	{
	public:
		CTcpEndpointTester(nm_smartnet::nm_rup::tcp_acceptor_ptr_t&);
		CTcpEndpointTester(nm_smartnet::nm_rup::tcp_connector_ptr_t&);
		virtual ~CTcpEndpointTester();

	public:
		void handle_pkg(nm_pkg::CPkgHB &, u_int32_t uiTag);

	protected:
		///
		virtual void on_opened();
		virtual void on_closed();
		virtual void on_recved_data(nm_mem::mem_ptr_t &pData);
	};
	typedef nm_utils::CSmartPtr<nm_busi::CTcpEndpointTester> tcp_ep_tester_ptr_t;

}

#endif /* TCP_ENDPOINT_TESTER_H_ */
