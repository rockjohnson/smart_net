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

	CTcpEndpointTester::CTcpEndpointTester(nm_smartnet::tcp_acceptor_ptr_t &pT)
	:nm_smartnet::CTcpEndpoint(pT)
	{
		// TODO Auto-generated constructor stub
	}

	CTcpEndpointTester::~CTcpEndpointTester()
	{
		// TODO Auto-generated destructor stub
	}

	void CTcpEndpointTester::on_opened(int32_t i32ErrCode)
	{
		std::cout<<"tester opened"<<std::endl;
	}

	void CTcpEndpointTester::on_closed()
	{
		std::cout<<"tester closed"<<std::endl;
	}

	void CTcpEndpointTester::on_recved_data(nm_mem::mem_ptr_t &pData)
	{

	}

}
