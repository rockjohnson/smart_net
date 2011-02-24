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

}

#endif /* CONN_H_ */
