/*
 * service.h
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 */

#ifndef __SERVICE_H__
#define __SERVICE_H__

#include <vector>

#include "../common/sn_common.h"
#include "../network/sn_net_addr.h"
#include "smart_net.h"

namespace nm_smartnet
{

using namespace nm_network;
using namespace nm_framework;

/**
 * the base net service interface
 *
 * you should separate your service with endpoint type.
 *
 * */
class INetService
{
public:
	INetService(net_engine_ptr_t &pNetEngine);
	virtual ~INetService();

public:
	virtual int32_t init() = 0;
	virtual int32_t destroy() = 0;

	virtual int32_t start() = 0;
	virtual int32_t stop() = 0;

private:
	///net engine.
	net_engine_ptr_t m_pNetEngine;
};

/**
 * tcp protocol service
 *
 * usage:
 *
 * CTcpService<CTcpConn> tcpServ;
 * CIpv4Addr localAddr, remoteAddr;
 * ...
 * tcpServ.init(localAddr, remoteAddr);
 * tcpServ.start();
 * ...
 * tcpServ.stop();
 * */
class CTcpService : public INetService
{
public:
	CTcpService(CSmartNet &smartnet);
	~CTcpService();

public:
	enum {NO_CONNECTIONS_LIMIT = -1};
	/**
	 * if localaddr is valid, then it is the listen service, otherwise.
	 * */
	int32_t init();
//	int32_t start();
//	int32_t stop();
	int32_t destroy();
	void set_backlog(int32_t i32Backlog);

public:
	template <class LISTENER>
	int32_t add_listen_service(INetAddr &listenAddr, int32_t i32Backlog);
	int32_t add_connect_service();
	int32_t stop();

private:

	///listen service members.
	typedef std::vector<tcp_listener_ptr_t> tcp_listener_vec_t;
	tcp_listener_vec_t m_vecTcpListener;

	///connect service members.
	//CTcpConnecter::tcp_connecter_ptr_t m_pTcpConnecter;
	u_int32_t m_ui32ConnectTimeout;
	int32_t   m_i32MaxConnectTimes;
	bool      m_bAutoReconnect;
	CIpv4Addr m_remoteAddr;
	int32_t m_i32MinOutboundConnCnt;
};

/**
 *
 * */
class CRudpService : public INetService
{

};

class CRmpService : public INetService
{

};

class CPgmService : public INetService
{

};


}

#endif /* SERVICE_H_ */
