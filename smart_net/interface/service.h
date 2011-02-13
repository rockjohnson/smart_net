/*
 * service.h
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 */

#ifndef __SERVICE_H__
#define __SERVICE_H__

#include "../common/sn_common.h"
#include "../network/net_addr.h"

namespace nm_smartnet
{

	using namespace nm_network;

/**
 * the base net service interface
 * */
class INetService
{
public:
	INetService();
	virtual ~INetService();

public:
	virtual int32_t start() = 0;
	virtual int32_t stop() = 0;
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
template <class CConn>
class CTcpService : public INetService
{
public:
	CTcpService();
	~CTcpService();

public:
	enum {NO_CONNECTIONS_LIMIT = -1};
	/**
	 * if localaddr is valid, then it is the listen service, otherwise.
	 * */
	int32_t init(INetAddr &localAddr, INetAddr &remoteAddr, int32_t i32MaxInboundConnectionCnt, int32_t i32MinOutboundConnectionCnt);
	int32_t start();
	int32_t stop();
	void set_backlog(int32_t i32Backlog);

private:
	int32_t start_listen_service();
	int32_t start_connect_service();

private:
	///local and remote address.
	CIpv4Addr m_localAddr;
	CIpv4Addr m_remoteAddr;
	///max connection count, NO_CONNECTIONS_LIMIT means no limit.
	int32_t m_i32MaxInboundConnCnt;
	int32_t m_i32MinOutboundConnCnt;
	///
	tcp_listener_ptr_t m_pTcpListener;
	///
	int32_t m_i32Backlog;
	///
	net_engine_ptr_t m_pNetEngine;
};

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
