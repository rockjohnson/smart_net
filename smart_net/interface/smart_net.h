/*
 * smart_net.h
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 */

#ifndef __SMART_NET_H__
#define __SMART_NET_H__

namespace nm_smartnet
{

class CSmartNet
{
public:
	CSmartNet();
	virtual ~CSmartNet();

public:
	template<class C>
	int32_t add_tcp_listen_service();
	template<class C>
	int32_t add_tcp_connect_service();

};

}

#endif /* SMART_NET_H_ */
