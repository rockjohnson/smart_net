/*
 * connector.h
 *
 *  Created on: Feb 12, 2011
 *      Author: rock
 */

#ifndef CONNECTOR_H_
#define CONNECTOR_H_

#include "../framework/sn_timer.h"
#include "../network/sn_net_addr.h"

namespace nm_network
{

class IConnecter : public nm_framework::IInternalTimer
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
	int32_t init(INetAddr &remoteAddr);
	int32_t destroy();

	CIpv4Addr m_remoteAddr;
};
typedef nm_utils::CSmartPtr<nm_network::CTcpConnecter> tcp_connecter_ptr_t;




}

#endif /* CONNECTOR_H_ */
