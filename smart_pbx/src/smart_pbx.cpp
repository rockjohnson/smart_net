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
	nm_smartnet::CSmartNet sn;
#define IO_THREAD_CNT (10)
	sn.init(IO_THREAD_CNT);
	sn.start();

	CDevService DevServ(sn);
	DevServ.init();
	///
	CIpv4Addr listenAddr;
#define LISTEN_ADDR ("0.0.0.0")
	listenAddr.set_ip(LISTEN_ADDR);
#define LISTEN_PORT (9999)
	listenAddr.set_port_hbo(LISTEN_PORT);
#define BACKLOG (10)
	DevServ.add_listen_service<CDevListener>(listenAddr, BACKLOG);


	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
	return 0;
}
