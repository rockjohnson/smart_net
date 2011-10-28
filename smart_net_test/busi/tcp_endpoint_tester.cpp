/*
 * tcp_endpoint_tester.cpp
 *
 *  Created on: Oct 10, 2011
 *      Author: rock
 */
#include <iostream>

#include "tcp_endpoint_tester.h"

namespace nm_busi
{

	CTcpEndpointTester::CTcpEndpointTester(nm_smartnet::tcp_acceptor_ptr_t &pT) :
		nm_smartnet::CTcpEndpoint(pT)
	{
		// TODO Auto-generated constructor stub
	}

	CTcpEndpointTester::CTcpEndpointTester(nm_smartnet::tcp_connector_ptr_t &pT) :
		nm_smartnet::CTcpEndpoint(pT)
	{
		// TODO Auto-generated constructor stub
	}

	CTcpEndpointTester::~CTcpEndpointTester()
	{
		// TODO Auto-generated destructor stub
	}

	void CTcpEndpointTester::on_opened()
	{
		std::cout << "tester opened" << std::endl;
		nm_pkg::CArchive<nm_pkg::CPkgHdr, nm_pkg::CPkgReg> ar(3);
		nm_pkg::CPkgReg *pPkg = ar.get_next_body();
		pPkg->set_id(10);
		send_data(ar.serialize());
	}

	void CTcpEndpointTester::on_closed()
	{
		std::cout << "tester closed" << std::endl;
		open();
	}

	void CTcpEndpointTester::on_recved_data(nm_mem::mem_ptr_t &pData)
	{
		DISPATCH(CTcpEndpointTester);
	}

	void CTcpEndpointTester::handle_pkg(nm_pkg::CPkgReg &pkg, u_int32_t uiTag)
	{
		static u_int64_t ui64 = 0;
		if (((++ui64) % 100000) == 0)
		{
			std::cout<< "recved : " << ui64 << pkg.get_id() << std::endl;
		}
		send_data();
		//std::cout<< "handle_pkg reg pkg: " << pkg.get_opcode() << ", id: "<< pkg.get_id()<< std::endl;
	}

}
