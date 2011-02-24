/*
 * conn.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef CONN_H_
#define CONN_H_

#include "../framework/sn_io_obj.h"
#include "../network/sn_socket.h"
#include "../network/sn_connecter.h"
#include <memory/mem.h>

namespace nm_framework
{

using namespace nm_memory;
using namespace nm_network;

/**
 * base endpoint class
 * */
class IEndpoint : public nm_framework::IIoObj
{
public:
	IEndpoint();
	virtual ~IEndpoint();

public:

public:
	virtual int32_t init() = 0;
	virtual int32_t destroy() = 0;
	virtual int32_t send_data(mem_ptr_t &pData) = 0; ///thread-safe send func.
	virtual int32_t close() = 0; ///close this endpoint, close the connection, and will not send or receive data.

protected:
	virtual void on_connected(INetAddr &remoteAddr) = 0;
	virtual void on_recved_data(mem_ptr_t &pData, INetAddr &srcAddr) = 0;
	virtual void on_occurred_err(int32_t iErrCode) = 0;
};


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
class CTcpEndpoint: public nm_framework::IEndpoint
{
public:
	CTcpEndpoint();
	virtual ~CTcpEndpoint();

public:
	virtual int32_t send_data(mem_ptr_t &pData);
	virtual int32_t close();

protected:
	void handle_input_evt(); ///handle input event.
	void handle_output_evt(); ///handle ouput event.
	void handle_error_evt(); ///handle error event.
	int32_t get_fd();

private:
	CTcpSock::tcp_sock_ptr_t m_pSock;
};
typedef nm_utils::CSmartPtr<nm_smartnet::CTcpEndpoint> tcp_endpoint_ptr_t;


/**
 * tcp connect endpoint.
 * */
class CTcpConnecter;
typedef nm_utils::CSmartPtr<CTcpConnecter> tcp_connecter_ptr_t;
class CTcpOutboundEndpoint : public CTcpEndpoint
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
