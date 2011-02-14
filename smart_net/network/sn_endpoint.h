/*
 * conn.h
 *
 *  Created on: Nov 30, 2010
 *      Author: rock
 */

#ifndef CONN_H_
#define CONN_H_

#include "../framework/io_obj.h"
#include "../network/socket.h"

namespace nm_network
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
	///
	virtual int32_t init() = 0;
	virtual int32_t close() = 0;
	virtual int32_t send_data(mem_ptr_t &pData) = 0;

protected:
	///
	virtual void handle_input_evt();
	virtual void handle_output_evt();
	virtual void handle_error_evt();
	virtual int32_t get_fd();
	///
	virtual void on_connected() = 0;
	virtual void on_recved_data(mem_ptr_t &pData) = 0;
	virtual void on_closed(int32_t iErrCode) = 0;
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
	virtual int send_data(mem_ptr_t &pData);

protected:
	virtual void on_connected();
	virtual void on_recved_data(mem_ptr_t &pData);
	virtual void on_closed(int32_t iErrCode);

private:
	tcp_sock_ptr_t m_pSock;
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
