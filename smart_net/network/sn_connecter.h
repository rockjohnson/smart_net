/*
 * connector.h
 *
 *  Created on: Feb 12, 2011
 *      Author: rock
 */

#ifndef CONNECTOR_H_
#define CONNECTOR_H_

#include "../framework/sn_io_obj.h"

namespace nm_network
{

class IConnecter : public nm_framework::IIoObj
{
public:
	IConnecter();
	virtual ~IConnecter();
};

class CTcpConnecter : public IConnecter
{
public:
	CTcpConnecter();
	~CTcpConnecter();

public:
	int32_t init();
	int32_t destroy();

protected:
	void handle_input_evt();
	void handle_output_evt();
	void handle_error_evt();
	int32_t get_fd();
};
typedef nm_utils::CSmartPtr<nm_network::CTcpConnecter> tcp_connecter_ptr_t;


}

#endif /* CONNECTOR_H_ */
