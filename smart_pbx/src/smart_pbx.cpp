//============================================================================
// Name        : smart_pbx.cpp
// Author      : rock
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
using namespace std;

#include "../business/sp_dev_service.h"
using namespace nm_business;

int main()
{
	///1,inherit endpoint

	///2,create network engine object.
	nm_smartnet::CSmartNet sn;
	sn.start(10, nm_framework::EIEN_EPOLL, 10);

	///3,bind endpoint with service


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
