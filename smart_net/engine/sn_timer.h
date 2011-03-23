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

namespace nm_engine
{

class IInternalTimer: public nm_base::ICommonBase
{
public:
	IInternalTimer();
	virtual ~IInternalTimer();

public:
	virtual void check(u_int64_t ui64curtimeus) = 0;
};
typedef nm_utils::CSmartPtr<IInternalTimer> internal_timer_ptr_t;

}

#endif /* SN_TIMER_H_ */
