/*
 * service.h
 *
 *  Created on: Feb 11, 2011
 *      Author: rock
 */

#ifndef __SERVICE_H__
#define __SERVICE_H__

namespace nm_smartnet {

class IService
{
public:
	IService();
	virtual ~IService();

public:
	virtual int32_t start() = 0;
	virtual int32_t stop() = 0;


};

/**
 * tcp listen service
 * */
class CTcpServerService : public IService
{

};

class CTcpClienttService : public IService
{

};

class CRudpService : public IService
{

};

class CRmpService : public IService
{

};

class CPgmService : public IService
{

};


}

#endif /* SERVICE_H_ */
