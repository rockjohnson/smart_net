//============================================================================
// Name        : smart_pbx.cpp
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
	///1,inherit endpoint

	///2,create network engine object.
	smart_net_ptr_t pSmartNet = SYS_NOTRW_NEW(CSmartNet);
	pSmartNet->start(10, nm_framework::EIEN_EPOLL, 10);

	///3,create endpoint
	CTcpEndpoint tcpIBEP(pSmartNet);
	net_addr_ptr_t pListenAddr = SYS_NOTRW_NEW(CIpv4Addr);
	pListenAddr->set_ip("127.0.0.1");
	pListenAddr->set_port_hbo(9999);
	tcpIBEP.open(pListenAddr, net_addr_ptr_t(NULL));


	///4,

	///the app service.
//	CDevService DevServ(sn);
//	CIpv4Addr listenAddr;
//	listenAddr.set_ip("0.0.0.0");
//	listenAddr.set_port_hbo(9999);
//	DevServ.start();

//	CSPService SPServ(sn);
//	CIpv4Addr remoteAddr;
//	remoteAddr.set_ip("127.0.0.1");
//	remoteAddr.set_port_hbo(9999);
//	SPServ.add_connect_service<CSPConnecter>(remoteAddr, 5, true);
//	SPServ.start();

	sleep(1000);

//	SPServ.stop();
//	DevServ.stop();

	sn.stop();

	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
