//============================================================================
// Name        : smart_net_test.cpp
// Author      : rock
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include <smart_net/smart_net.h>
#include "../business/sp_dev_service.h"
using namespace nm_business;
using namespace nm_smartnet;

int main()
{
	///2,create network engine object.
	smart_net_ptr_t pSmartNet = SYS_NOTRW_NEW(CSmartNet);
	pSmartNet->start(10, nm_framework::EIEN_EPOLL, 10);

	///3,create endpoint
	tcp_ib_endpoint_ptr_t pTcpIBEP = SYS_NOTRW_NEW(CTcpInboundEndpoint);
	pTcpIBEP->attach(pSmartNet);

	net_addr_ptr_t pListenAddr = SYS_NOTRW_NEW(CIpv4Addr);
	pListenAddr->set_ip("127.0.0.1");
	pListenAddr->set_port_hbo(9999);

	pTcpIBEP->open(pListenAddr, net_addr_ptr_t(NULL));

	pTcpIBEP->close();
	pTcpIBEP->detach(pSmartNet);


	pSmartNet->stop();

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
