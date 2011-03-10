/*
 * sn_misc_task.h
 *
 *  Created on: Mar 8, 2011
 *      Author: rock
 */

#ifndef __SN_MISC_TASK_H__
#define __SN_MISC_TASK_H__

#include <thread/thread_ex.h>

namespace nm_engine
{

class CMiscTask : public nm_thread::IThreadTask
{
public:
	CMiscTask();
	virtual ~CMiscTask();
};
typedef nm_utils::CSmartPtr<CMiscTask> misc_task_ptr_t;

}

#endif /* __SN_MISC_TASK_H__ */
