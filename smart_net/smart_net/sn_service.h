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
#include "../network/sn_listener.h"

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
typedef nm_utils::CObjFactory<nm_smartnet::CTcpEndpoint>::obj_factory_ptr_t endpoint_factory_ptr_t;
class CTcpService : public INetService
{
public:
	CTcpService(CSmartNet &smartnet);
	virtual ~CTcpService();

public:
	///
	virtual int32_t start(net_addr_ptr_t &pLocalNetAddr, net_addr_ptr_t &pPeerNetAddr, endpoint_factory_ptr_t &pEndpointFactory);
	virtual int32_t stop();
	///for listen service
	void set_listen_serv_info(int32_t i32Backlog);
	///for connect service
	void set_connect_serv_info(int32_t i32ConnTimeout, int32_t i32MaxRetries);

protected:
	int32_t start_listen_service(net_addr_ptr_t &pBindAddr, int32_t i32Backlog);
	int32_t start_connect_service(net_addr_ptr_t &pRemoteAddr, int32_t i32ConnTimeout, int32_t i32MaxRetries);

private:
	endpoint_factory_ptr_t m_pEndpointFactory;
	///listen service members.
	tcp_listener_ptr_t m_pTcpListener;
	int32_t m_i32Backlog;
	///connect service members.
	tcp_ob_endpoint_ptr_t m_pOutboundEndpoint;
	int32_t m_i32ConnTimeout;
	int32_t m_i32MaxRetries;
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
