/*
 * dev_service.h
 *
 *  Created on: Feb 16, 2011
 *      Author: rock
 */

#ifndef DEV_SERVICE_H_
#define DEV_SERVICE_H_

#include <smart_net/sn_endpoint.h>
#include <smart_net/sn_service.h>

namespace nm_business
{

class CDevEndpoint : public nm_smartnet::CTcpEndpoint
{
public:
	CDevEndpoint();
	~CDevEndpoint();

public:
	///object init and destroy;
	int32_t init();
	int32_t destroy();

	///close the current endpoint, but not destroy this obj.
	int32_t close();

	///thread-safe func.
	int32_t send_data(mem_ptr_t &pMem);
protected:
	///callback func which invoked by net io thread.
	void on_connected();
	void on_recved_data(mem_ptr_t &pMem);
	void on_net_error();
};

/**
 * device service.
 * */
class CDevService : public nm_smartnet::CTcpService<nm_business::CDevEndpoint>
{
public:
	CDevService();
	virtual ~CDevService();
};

}

#endif /* DEV_SERVICE_H_ */
