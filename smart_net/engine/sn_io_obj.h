/*
 * io_obj.h
 *
 *  Created on: Dec 29, 2010
 *      Author: rock
 */

#ifndef __IO_OBJ_H__
#define __IO_OBJ_H__

#include <utils/smart_ptr.h>

#include "../common/sn_common.h"

namespace nm_engine
{

class CSelect;
class CEpoll;
/**
 * base class for io objs.
 * */
class IIoObj : public nm_base::ICommonBase
{
	friend class CSelect;
	friend class CEpoll;
public:
	IIoObj(){}
	virtual ~IIoObj(){}

protected:
	virtual void handle_input_evt() = 0;
	virtual void handle_output_evt() = 0;
	virtual void handle_error_evt() = 0;
	virtual sock_handle_t get_handle() = 0;
	virtual u_int32_t get_input_evts() = 0;
	virtual u_int32_t get_output_evts() = 0;
	virtual u_int32_t get_misc_evts() = 0;
};
typedef nm_utils::CSmartPtr<nm_framework::IIoObj> io_obj_ptr_t;

}

#endif /* __IO_OBJ_H__ */
