/*
 * main.cpp
 *
 *  Created on: Oct 10, 2011
 *      Author: rock
 */

#include <iostream>
using namespace std;

#include "busi/tcp_endpoint_tester.h"

int main()
{
	SET_PKG_HANDLER(nm_busi::CTcpEndpointTester, nm_pkg::CPkgHB);

	nm_framework::sn_engine_ptr_t pSNEngine = SYS_NOTRW_NEW(nm_framework::CSNEngine);
	IF_TRUE_THEN_RETURN_CODE(pSNEngine->start(2, 2, nm_framework::EIEN_EPOLL, 100) < 0, -1);

#if 1
	std::cout<<"ok1"<<std::endl;

	nm_smartnet::nm_rup::tcp_acceptor_ptr_t pTcpAcceptor = SYS_NOTRW_NEW(nm_smartnet::nm_rup::CRupAcceptor(pSNEngine));

	std::cout<<"ok2"<<std::endl;
	IF_TRUE_THEN_RETURN_CODE(pTcpAcceptor->open(cmn_string_t("0.0.0.0"), 8888) < 0, -1);
	std::cout<<"ok3"<<std::endl;

	nm_smartnet::nm_rup::tcp_connector_ptr_t pTcpConnector = SYS_NOTRW_NEW(nm_smartnet::nm_rup::CRupConnector(pSNEngine));
	std::cout<<"ok4"<<std::endl;
	IF_TRUE_THEN_RETURN_CODE(pTcpConnector->open(cmn_string_t("127.0.0.1"), 8888, 1000000) < 0, -1);
	std::cout<<"ok5"<<std::endl;

//	std::cout<<"ok3"<<std::endl;
	nm_busi::tcp_ep_tester_ptr_t pA = SYS_NOTRW_NEW(nm_busi::CTcpEndpointTester(pTcpAcceptor));
//	nm_busi::tcp_ep_tester_ptr_t pC = SYS_NOTRW_NEW(nm_busi::CTcpEndpointTester(pTcpAcceptor));
//	nm_busi::tcp_ep_tester_ptr_t pD = SYS_NOTRW_NEW(nm_busi::CTcpEndpointTester(pTcpAcceptor));
//	nm_busi::tcp_ep_tester_ptr_t pE = SYS_NOTRW_NEW(nm_busi::CTcpEndpointTester(pTcpAcceptor));

	pA->open();
//	pC->open();
//	pD->open();
//	pE->open();
	nm_busi::tcp_ep_tester_ptr_t pB = SYS_NOTRW_NEW(nm_busi::CTcpEndpointTester(pTcpConnector));
	pB->open();

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
	tcp_ib_acceptor_ptr_t pTcpIBAcceptor = SYS_NOTRW_NEW(CRupAcceptor(pSmartNet));
	pTcpIBAcceptor->open(std::string("0.0.0.1"), 9999);

	///3,create endpoint
	rup_endpoint_ptr_t pTcpIBEP = SYS_NOTRW_NEW(CRupEndpoint(pTcpIBAcceptor));
	pTcpIBEP->open();

	///1,create outbound connector
	tcp_ob_connector_ptr_t pTcpOBConnector = SYS_NOTHRW_NEW(CRupConnector(pSmartNet));
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

	while (!pB->is_opened())
	{
		sleep(1);
	}

	int32_t i32Cnt = 0;
	int32_t i32Ret = 0;
	while (i32Cnt++ < 1000009)
	{
		if (pB->is_opened())
		{
//			nm_pkg::CArchive<nm_pkg::CPkgHdr, nm_pkg::CPkgHB> ar(3);
//			nm_pkg::CPkgHB *pPkg = ar.get_next_body();
//			pPkg->set_id(i32Cnt);
////			nm_pkg::CPkgReg *pPkg1 = ar.get_next_body();
////			pPkg1->set_id(2);
//			i32Ret = pB->send_data(ar.serialize());
//			if (CMNERR_SEND_PENDING)
//			{
//				cout << "!!!1 World!!!" << endl; // prints !!!Hello World!!!
//			}
//			else if (CMNERR_SUC == i32Ret)
//			{
//				cout << "!!!2 World!!!" << endl; // prints !!!Hello World!!!
//			}
//			else
//			{
//				cout << "!!!3 World!!!" << endl; // prints !!!Hello World!!!
//			}
		}
	}

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!

	sleep(10000000);
	return 0;
}
