/*
 * main.cpp
 *
 *  Created on: Oct 10, 2011
 *      Author: rock
 */

#include <iostream>
using namespace std;

#include "../busi/tcp_endpoint_tester.h"

int main()
{
	nm_framework::sn_engine_ptr_t pSNEngine = SYS_NOTRW_NEW(nm_framework::CSNEngine);
	IF_TRUE_THEN_RETURN_CODE(pSNEngine->start(1, 1, nm_framework::EIEN_EPOLL, 100) < 0, -1);

#if 1
	std::cout<<"ok1"<<std::endl;

	nm_smartnet::tcp_acceptor_ptr_t pTcpAcceptor = SYS_NOTRW_NEW(nm_smartnet::CTcpAcceptor(pSNEngine));

	std::cout<<"ok2"<<std::endl;
	IF_TRUE_THEN_RETURN_CODE(pTcpAcceptor->open(cmn_string_t("0.0.0.0"), 9999) < 0, -1);

	std::cout<<"ok3"<<std::endl;

	nm_smartnet::tcp_connector_ptr_t pTcpConnector = SYS_NOTRW_NEW(nm_smartnet::CTcpConnector(pSNEngine));
	std::cout<<"ok4"<<std::endl;
	IF_TRUE_THEN_RETURN_CODE(pTcpConnector->open(cmn_string_t("127.0.0.1"), 9999, 1000000) < 0, -1);
	std::cout<<"ok5"<<std::endl;

//	std::cout<<"ok3"<<std::endl;
	nm_busi::tcp_ep_tester_ptr_t pA = SYS_NOTRW_NEW(nm_busi::CTcpEndpointTester(pTcpAcceptor));
	pA->open();
//	nm_busi::tcp_ep_tester_ptr_t pB = SYS_NOTRW_NEW(nm_busi::CTcpEndpointTester(pTcpConnector));
//	pB->open();

	sleep(10);

//	pB->close();
//	pA->close();
//	pB->open();
//	pA->open();

#endif

	//	pA->open();
	//	pB->open();
	//
	//	::sleep(100);
	//
	//	pA->close();
	//	pB->close();
	//
	//	pTcpAcceptor->close();
	//	pTcpConnector->close();
	//	pSNEngine->stop();

#if (0)
	///1,create network engine object.
	smart_net_ptr_t pSmartNet = SYS_NOTRW_NEW(CSmartNet);
	pSmartNet->start(10, 10,nm_framework::EIEN_EPOLL, 10);

	///2,create acceptor
	tcp_ib_acceptor_ptr_t pTcpIBAcceptor = SYS_NOTRW_NEW(CTcpAcceptor(pSmartNet));
	pTcpIBAcceptor->open(std::string("0.0.0.1"), 9999);

	///3,create endpoint
	tcp_endpoint_ptr_t pTcpIBEP = SYS_NOTRW_NEW(CTcpEndpoint(pTcpIBAcceptor));
	pTcpIBEP->open();

	///1,create outbound connector
	tcp_ob_connector_ptr_t pTcpOBConnector = SYS_NOTHRW_NEW(CTcpConnector(pSmartNet));
	pTcpOBConnector->open(std::string("127.0.0.1"), 9999);

	///2,create outbound endpoint
	tcp_ob_endpoint_ptr_t pTcpOBEP = SYS_NOTHRW_NEW(CTcpOutboundEndpoint(pTcpOBConnector));
	pTcpOBEP->open();

	///1,create rmp endpoint
	rmp_endpoint_ptr_t pRmpEP = SYS_NOTHRW_NEW(CRmpEndpoint(pSmartNet));
	pRmpEP->open(std::string("224.0.2.99"), std::string("127.0.0.1"), 9999);

	::sleep(100000);

	pTcpIBEP->close();

	pSmartNet->stop();

#endif

	while (1)
	{

	sleep(100000000);

	}
	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
