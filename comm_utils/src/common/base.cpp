/*
 * base.cpp
 *
 *  Created on: Feb 15, 2011
 *      Author: rock
 */

#include "base.h"
#include "../utils/atomic.h"

namespace nm_cmn_base
{

using namespace nm_utils;

ICommonBase::ICommonBase() :
	m_iCnt(0)
{
}

ICommonBase::~ICommonBase()
{
	CMN_ASSERT(m_iCnt == 0);
}

void ICommonBase::inc_ref()
{
	atomic_increment(&m_iCnt);
}

void ICommonBase::dec_ref()
{
	CMN_ASSERT(m_iCnt> 0);
	if (atomic_exchange_and_add(&m_iCnt, -1) == 1)
	{
		dispose_this_obj();
	}
}

void ICommonBase::dispose_this_obj()
{
	delete this;
}

int ICommonBase::get_ref_cnt()
{
	return m_iCnt;
}

ICommonBase::ICommonBase(const ICommonBase &other)
{
}
ICommonBase& ICommonBase::operator =(const ICommonBase&)
{
	return *this;
}
}
