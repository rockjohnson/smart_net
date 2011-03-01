/*
 * sn_timer.h
 *
 *  Created on: Feb 23, 2011
 *      Author: rock
 */

#ifndef SN_TIMER_H_
#define SN_TIMER_H_

#include "../common/sn_common.h"

namespace nm_framework
{

class IInternalTimer : public nm_base::ICommonBase
{
public:
	IInternalTimer();
	virtual ~IInternalTimer();

public:
	virtual int32_t check(int32_t i32CurMSTime) = 0;
};
typedef nm_utils::CSmartPtr<IInternalTimer> internal_timer_ptr_t;

}

#endif /* SN_TIMER_H_ */
