/*
 * sn_connecter.h
 *
 *  Created on: Feb 24, 2011
 *      Author: rock
 */

#ifndef SN_CONNECTER_H_
#define SN_CONNECTER_H_

namespace nm_network
{

class IConnecter
{
public:
	IConnecter();
	virtual ~IConnecter();
};


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

}

#endif /* SN_CONNECTER_H_ */
