/*
 * service.h
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 */

#ifndef __SERVICE_H__
#define __SERVICE_H__

#include <vector>
#include <utils/obj_factory.h>
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
	virtual int32_t start(net_addr_ptr_t &pLocalNetAddr, net_addr_ptr_t &pPeerNetAddr);
	virtual int32_t stop() = 0;

protected:
	net_engine_ptr_t m_pNetEngine; ///net engine.
	net_addr_ptr_t m_pLocalNetAddr;
	net_addr_ptr_t m_pPeereNetAddr;
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
template <class ENDPOINT, class ENDPOINT_FACTORY = nm_utils::CObjFactory<ENDPOINT> >
class CTcpService : public INetService
{
public:
	CTcpService(CSmartNet &smartnet);
	~CTcpService();

public:
	enum {NO_CONNECTIONS_LIMIT = -1};

	void set_backlog(int32_t i32Backlog);

public:
	virtual int32_t start(net_addr_ptr_t &pLocalNetAddr, net_addr_ptr_t &pPeerNetAddr);

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
	ENDPOINT_FACTORY m_EndpointFactory;
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
