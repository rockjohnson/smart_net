/*
 * connector.h
 *
 *  Created on: Feb 12, 2011
 *      Author: rock
 */

#ifndef CONNECTOR_H_
#define CONNECTOR_H_

#include "../framework/io_obj.h"

namespace nm_network
{

class IConnecter : public nm_framework::CIoObj
{
public:
	IConnecter();
	virtual ~IConnecter();
};

class CTcpConnecter : public IConnecter
{

};

}

#endif /* CONNECTOR_H_ */
