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

class ITimerObj: public virtual nm_cmn_base::ICommonBase
{
public:
	ITimerObj();
	virtual ~ITimerObj();

public:
	virtual void check(u_int64_t ui64curtimeus) = 0;
};
typedef nm_utils::CSmartPtr<ITimerObj> timer_obj_ptr_t;

}

#endif /* SN_TIMER_H_ */
