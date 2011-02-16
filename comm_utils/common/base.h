/*
 * noncopyable.h
 *
 *  Created on: Apr 5, 2009
 *      Author: rock
 */

#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

#include "types.h"

namespace nm_base
{
class CNoncopyable
{
protected:
	CNoncopyable()
	{
	}
	virtual ~CNoncopyable()
	{
	}
private:
	// emphasize the following members are private
	CNoncopyable(const CNoncopyable&);
	const CNoncopyable& operator=(const CNoncopyable&);
};

class CNonnewobj
{
private:
	CNonnewobj()
	{
	}
	CNonnewobj(const CNonnewobj &t)
	{
	}
public:
	virtual ~CNonnewobj()
	{
	}
};

/**
 * common base
 * */
class ICommonBase
{
protected:
	ICommonBase();
	virtual ~ICommonBase();

public:
	inline void inc_ref();
	inline void dec_ref();
	virtual void dispose_this_obj();
	inline int get_ref_cnt();

private:
	ICommonBase(const ICommonBase &other);
	ICommonBase& operator =(const ICommonBase&);

private:
	int32_t m_iCnt;
};

}

#endif /* __NONCOPYABLE_H__ */
