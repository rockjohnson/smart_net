/*
 * sn_misc_task.h
 *
 *  Created on: Mar 8, 2011
 *      Author: rock
 */

#ifndef __SN_MISC_TASK_H__
#define __SN_MISC_TASK_H__

#include <thread/thread_ex.h>

namespace nm_framework
{

class CMiscTask : public nm_thread::IThreadTask
{
public:
	CMiscTask();
	virtual ~CMiscTask();
};

}

#endif /* __SN_MISC_TASK_H__ */
