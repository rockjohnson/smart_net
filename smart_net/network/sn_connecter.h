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

/**
 * tcp connect timer.
 * */
class CTcpConnecter : public nm_framework::IInternalTimer
{
public:
	CTcpConnecter();
	~CTcpConnecter();

public:
	int32_t open(const net_addr_ptr_t &pDstAddr);
	int32_t close();

	net_addr_ptr_t m_pDstAddr;
};

}

#endif /* SN_CONNECTER_H_ */
