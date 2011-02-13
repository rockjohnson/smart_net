/*
 * io_obj.h
 *
 *  Created on: Dec 29, 2010
 *      Author: rock
 */

#ifndef _IO_OBJ_H___
#define _IO_OBJ_H___

#include "../common/sn_common.h"

namespace nm_framework
{

class CIotask;

class IIoObj __SUPPORT_REF_CNT__
{
	friend class CIoTask;
public:
	IIoObj();
	virtual ~IIoObj();

protected:
	virtual void handle_read_evt() = 0;
	virtual void handle_write_evt() = 0;
	virtual void handle_error_evt() = 0;
	virtual int32_t get_fd() = 0;
};

typedef nm_utils::CSmartPtr<nm_framework::IIoObj> io_obj_ptr_t;

}

#endif /* __IO_OBJ_H__ */
