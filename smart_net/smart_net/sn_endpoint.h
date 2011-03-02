/*
 * conn.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef CONN_H_
#define CONN_H_

#include "../framework/sn_base.h"

namespace nm_smartnet
{

using namespace nm_memory;
using namespace nm_network;

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
	CTcpInboundEndpoint(smart_net_ptr_t &pSmartNet);
	virtual ~CTcpInboundEndpoint();

public:
	virtual int32_t open(const net_addr_ptr_t &pListenAddr, const net_addr_ptr_t &pPeerAddr); ///open this endpoint.
	virtual int32_t close(); ///close this endpoint.
	virtual int32_t send_data(mem_ptr_t &pData);
			int32_t get_type();

protected:
	void handle_input_evt(); ///handle input event.
	void handle_output_evt(); ///handle ouput event.
	void handle_error_evt(); ///handle error event.
	int32_t get_fd();

private:
	CTcpSock::tcp_sock_ptr_t m_pSock;
};
typedef nm_utils::CSmartPtr<nm_smartnet::CTcpInboundEndpoint> tcp_endpoint_ptr_t;


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
