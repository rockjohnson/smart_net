/*
 * conn.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef CONN_H_
#define CONN_H_

#include "../framework/sn_mgr.h"

namespace nm_network
{

/**
 * tcp endpoint enum.
 * */
enum ETcpEndpointType
{
	E_TCP_INBOUND_ENDPOINT = 0,
	E_TCP_OUTBOUND_ENDPOINT
};

/**
 * tcp endpoint
 * */
class CTcpInboundEndpoint: public IEndpoint
{
public:
	CTcpInboundEndpoint();
	virtual ~CTcpInboundEndpoint();

public:
	///first, you should open this endpoint, but it is async. and if succeed, then on_opened will callback.
	virtual int32_t open(const net_addr_ptr_t &pListenAddr, const net_addr_ptr_t &pPeerAddr, const nm_framework::smart_net_mgr_ptr_t psmartnetmgr);
	///if you want close this endpoint, please invoke this func, but it is aysnc.
	virtual int32_t close();
	///send data, async.
	virtual int32_t send_data(mem_ptr_t &pData);
			int32_t get_type();
	net_addr_ptr_t& get_listen_addr();

protected:
	virtual void handle_input_evt(); ///handle input event.
	virtual void handle_output_evt(); ///handle ouput event.
	virtual void handle_error_evt(); ///handle error event.
	///
	virtual void on_opened() = 0;
	virtual void on_recved_data(mem_ptr_t &pData) = 0;
	virtual void on_io_error(int32_t i32ErrCode) = 0;
	///
	int32_t get_fd();

private:
	nm_utils::CMutexLock m_lkendpoint;
	nm_framework::smart_net_mgr_ptr_t m_psmartnetmgr;
	CTcpSock::tcp_sock_ptr_t m_psock;
	net_addr_ptr_t m_plistenaddr;
	net_addr_ptr_t m_ppeeraddr;
};
typedef nm_utils::CSmartPtr<nm_smartnet::CTcpInboundEndpoint> tcp_ib_endpoint_ptr_t;


/**
 * tcp connect endpoint.
 * */
class CTcpConnecter;
typedef nm_utils::CSmartPtr<CTcpConnecter> tcp_connecter_ptr_t;
class CTcpOutboundEndpoint : public CTcpInboundEndpoint
{
public:
	CTcpOutboundEndpoint();
	virtual ~CTcpOutboundEndpoint();

public:
	int32_t start(net_addr_ptr_t &pRemoteAddr, int32_t i32ConnTimeout, int32_t i32MaxRetries);
	int32_t stop();

protected:
	void on_closed(int32_t iErrCode);

private:
	net_addr_ptr_t m_pRemoteAddr;
	tcp_connecter_ptr_t m_pConnecter;
};
typedef nm_utils::CSmartPtr<nm_smartnet::CTcpOutboundEndpoint> tcp_ob_endpoint_ptr_t;


}

#endif /* CONN_H_ */
