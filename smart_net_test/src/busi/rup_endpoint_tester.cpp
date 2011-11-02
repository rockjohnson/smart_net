/*
 * tcp_endpoint_tester.cpp
 *
 *  Created on: Oct 10, 2011
 *      Author: rock
 */
#include <iostream>

#include "rup_endpoint_tester.h"

namespace nm_busi
{

	CRupEndpointTester::CRupEndpointTester(nm_smartnet::nm_rup::tcp_acceptor_ptr_t &pT) :
		nm_smartnet::nm_rup::CRupEndpoint(pT)
	{
		// TODO Auto-generated constructor stub
	}

	CRupEndpointTester::CRupEndpointTester(nm_smartnet::nm_rup::tcp_connector_ptr_t &pT) :
		nm_smartnet::nm_rup::CRupEndpoint(pT)
	{
		// TODO Auto-generated constructor stub
	}

	CRupEndpointTester::~CRupEndpointTester()
	{
		// TODO Auto-generated destructor stub
	}

	void CRupEndpointTester::on_opened()
	{
		std::cout << "tester opened" << std::endl;
//		nm_pkg::CArchive<nm_pkg::CPkgHdr, nm_pkg::CPkgHB> ar(3);
//		nm_pkg::CPkgHB *pPkg = ar.get_next_body();
//		pPkg->set_id(10);
//		send_data(ar.serialize());
	}

	void CRupEndpointTester::on_closed()
	{
		std::cout << "tester closed" << std::endl;
		open();
	}

	void CRupEndpointTester::on_recved_data(nm_mem::mem_ptr_t &pData)
	{
		DISPATCH(CRupEndpointTester);
	}

	void CRupEndpointTester::handle_pkg(nm_pkg::CPkgHB &pkg, u_int32_t uiTag)
	{
		//
//		static u_int64_t ui64 = 0;
//		if (((++ui64) % 100000) == 0)
//		{
//			std::cout<< "recved : " << ui64 << pkg.get_id() << std::endl;
//		}
		//send_data();
		//std::cout<< "handle_pkg reg pkg: " << pkg.get_opcode() << ", id: "<< pkg.get_id()<< std::endl;
	}
}
