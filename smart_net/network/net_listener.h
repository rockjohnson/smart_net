/*
 * net_listener.h
 *
 *  Created on: Feb 12, 2011
 *      Author: rock
 */

#ifndef NET_LISTENER_H_
#define NET_LISTENER_H_

#include "../framework/io_obj.h"

namespace nm_network
{

/**
 * base net listner
 * */
class INetListener : public nm_framework::CIoObj
{
public:
	INetListener();
	virtual ~INetListener();
};

class CTcpListener : public INetListener
{

};

}

#endif /* NET_LISTENER_H_ */
