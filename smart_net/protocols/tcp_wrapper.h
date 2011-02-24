/*
 * tcp_wrapper.h
 *
 *  Created on: Feb 24, 2011
 *      Author: rock
 */

#ifndef TCP_WRAPPER_H_
#define TCP_WRAPPER_H_

#include "../framework/sn_socket.h"
#include "../framework/sn_timer.h"
#include "../framework/sn_io_obj.h"

namespace nm_protocol
{

/**
 * tcp socket
 *
 * */
class CTcpSock: public nm_network::ISocket
{
public:
	typedef nm_utils::CSmartPtr<nm_network::CTcpSock> tcp_sock_ptr_t;

public:
	CTcpSock();
	virtual ~CTcpSock();

public:
	int32_t create(int32_t i32fd = INVALID_SOCKET);
	int32_t close();
	int32_t bind(INetAddr &localAddr);
	int32_t listen(int32_t i32Backlog);
	tcp_sock_ptr_t accept();
	int32_t connect(INetAddr &remoteAddr);
	int32_t get_fd();
	bool is_valid();
	int32_t set_nonblock(bool bFlag);

private:
	int32_t m_i32fd;
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
class CTcpEndpoint: public IEndpoint
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

};
typedef nm_utils::CSmartPtr<nm_smartnet::CTcpOutboundEndpoint> tcp_ob_endpoint_ptr_t;

/**
 * tcp connect timer.
 * */
class CTcpConnecter : public nm_framework::IInternalTimer
{
public:
	CTcpConnecter();
	~CTcpConnecter();

public:
	int32_t init(INetAddr &remoteAddr);
	int32_t destroy();

	CIpv4Addr m_remoteAddr;
};
typedef nm_utils::CSmartPtr<nm_network::CTcpConnecter> tcp_connecter_ptr_t;


/**
 * tcp listener
 * */
class CTcpListener : public nm_framework::IIoObj
{
public:
	CTcpListener();
	virtual ~CTcpListener();

public:
	///
	int32_t start(net_addr_ptr_t &pBindAddr, int32_t i32Backlog, net_engine_ptr_t &pNetEngine);
	int32_t stop();
	///
	void handle_input_evt();
	void handle_output_evt();
	void handle_error_evt();
	int32_t get_fd();

private:
	CTcpSock m_tcpSock;
	net_addr_ptr_t m_pBindAddr;
	net_engine_ptr_t m_pNetEngine;
};
typedef nm_utils::CSmartPtr<nm_smartnet::CTcpListener> tcp_listener_ptr_t;

}

#endif /* TCP_WRAPPER_H_ */
