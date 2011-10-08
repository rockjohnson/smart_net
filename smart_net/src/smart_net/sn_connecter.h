/*
 * sn_connecter.h
 *
 *  Created on: Feb 24, 2011
 *      Author: rock
 */

#ifndef SN_CONNECTER_H_
#define SN_CONNECTER_H_

#include <string>

#include "../framework/sn_timer.h"

namespace nm_network
{

/**
 * tcp connect timer.
 * */
class CTcpOutboundConnector : public nm_framework::IInternalTimer
{
public:
	CTcpOutboundConnector();
	virtual ~CTcpOutboundConnector();

public:
	int32_t open(const std::string &strAcceptorIP, u_int16_t ui16AcceptorPort);
	int32_t close();
};

}

#endif /* SN_CONNECTER_H_ */
