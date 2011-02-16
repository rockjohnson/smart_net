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
#include "sn_connecter.h"

namespace nm_smartnet
{

/**
 * base endpoint class
 * */
class IEndpoint : public nm_framework::IIoObj
{
public:
	IEndpoint();
	virtual ~IEndpoint();

public:
	virtual int32_t send_data(mem_ptr_t &pData) = 0; ///thread-safe send func.
	virtual int32_t close() = 0; ///close this endpoint, close the connection, and will not send or receive data.

protected:
	virtual void on_connected() = 0;
	virtual void on_recved_data(mem_ptr_t &pData) = 0;
	virtual void on_occurred_err(int32_t iErrCode) = 0;
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
	tcp_sock_ptr_t m_pSock;
};

/**
 * tcp connect endpoint.
 * */
class CTcpOutboundEndpoint : public CTcpEndpoint
{
public:
	CTcpOutboundEndpoint();
	~CTcpOutboundEndpoint();

public:
	int32_t start();
	int32_t stop();

protected:
	void on_closed(int32_t iErrCode);

private:
	tcp_connecter_ptr_t m_pConnecter;
};

/**
 * udp endpoint
 * */
class CUdpEndpoint: public IEndpoint
{
public:
	CUdpEndpoint();
	virtual ~CUdpEndpoint();
};

/**
 * rmp endpoint
 * */
class CRmpEndpoint: public IEndpoint
{
public:
	CRmpEndpoint();
	virtual ~CRmpEndpoint();
};

}

#endif /* CONN_H_ */
