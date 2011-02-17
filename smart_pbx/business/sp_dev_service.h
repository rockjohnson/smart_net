/*
 * dev_service.h
 *
 *  Created on: Feb 16, 2011
 *      Author: rock
 */

#ifndef DEV_SERVICE_H_
#define DEV_SERVICE_H_

#include <smart_net/sn_listener.h>
#include <smart_net/sn_service.h>

namespace nm_business
{

using namespace nm_smartnet;

/**
 * endpoint.
 * */
class CDevEndpoint : public CTcpEndpoint
{
public:
	CDevEndpoint();
	~CDevEndpoint();

public:
	int32_t init();
	int32_t destroy();
	int32_t close();

	int32_t send_data(mem_ptr_t &pMem); ///thread-safe func.
protected:
	void on_connected();
	void on_recved_data(mem_ptr_t &pMem);
	void on_net_error();
};

/**
 * listener
 * */
class CDevListener : public CTcpListener
{
public:
	CDevListener();
	~CDevListener();

protected:
	tcp_endpoint_ptr_t create_endpoint();
};

/**
 * device service.
 * */
class CDevService : public CTcpService<CDevEndpoint>
{
public:
	CDevService(CSmartNet &smartnet);
	virtual ~CDevService();

public:
	int32_t start();
	int32_t stop();
};

}

#endif /* DEV_SERVICE_H_ */
