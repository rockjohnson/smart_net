/*
 * sn_timer.h
 *
 *  Created on: Feb 23, 2011
 *      Author: rock
 */

#ifndef SN_TIMER_H_
#define SN_TIMER_H_

#include <utils/smart_ptr.h>

#include "../common/sn_common.h"

namespace nm_framework
{

class ITimer: public nm_base::ICommonBase
{
public:
	ITimer();
	virtual ~ITimer();

public:
	virtual int32_t check(int32_t i32CurMSTime) = 0;
};
typedef nm_utils::CSmartPtr<ITimer> timer_ptr_t;

}

#endif /* SN_TIMER_H_ */
