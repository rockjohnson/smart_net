//============================================================================
// Name        : smart_net_test.cpp
// Author      : rock
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
using namespace std;

#include <smart_net/smart_net.h>

using namespace nm_business;
using namespace nm_smartnet;

int main()
{
	///1,create network engine object.
	smart_net_ptr_t pSmartNet = SYS_NOTRW_NEW(CSmartNet);
	pSmartNet->start(10, 10,nm_framework::EIEN_EPOLL, 10);

	///2,create acceptor
	tcp_ib_acceptor_ptr_t pTcpIBAcceptor = SYS_NOTRW_NEW(CTcpInboundAcceptor(pSmartNet));
	pTcpIBAcceptor->open(std::string("0.0.0.1"), 9999);

	///3,create endpoint
	tcp_ib_endpoint_ptr_t pTcpIBEP = SYS_NOTRW_NEW(CTcpInboundEndpoint(pTcpIBAcceptor));
	pTcpIBEP->open();

	///1,create outbound connector
	tcp_ob_connector_ptr_t pTcpOBConnector = SYS_NOTHRW_NEW(CTcpOutboundConnector(pSmartNet));
	pTcpOBConnector->open(std::string("127.0.0.1"), 9999);

	///2,create outbound endpoint
	tcp_ob_endpoint_ptr_t pTcpOBEP = SYS_NOTHRW_NEW(CTcpOutboundEndpoint(pTcpOBConnector));
	pTcpOBEP->open();

	::sleep(100000);

	pTcpIBEP->close();

	pSmartNet->stop();

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
