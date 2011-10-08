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

	enum E_IO_TYPE
	{
		EIT_INPUT = 1, EIT_OUTPUT
	};

	/**
	 * base class for io objs.
	 * */
	class IIoObj: public nm_comm_base::ICommonBase
	{
	public:
		IIoObj()
		{
		}
		virtual ~IIoObj()
		{
		}

	public:
		virtual void handle_input_evt() = 0;
		virtual void handle_output_evt() = 0;
		virtual void handle_error_evt() = 0;
		virtual sock_handle_t get_handle() = 0;

		virtual u_int32_t get_input_evts() = 0;
		virtual u_int32_t get_output_evts() = 0;
		virtual u_int32_t get_misc_evts() = 0;
		virtual void set_input_task_id(int32_t i32id) = 0;
		virtual int32_t get_input_task_id() = 0;
		virtual void set_output_task_id(int32_t i32id) = 0;
		virtual int32_t get_output_task_id() = 0;
	};
	typedef nm_utils::CSmartPtr<IIoObj> io_obj_ptr_t;

}

#endif /* __IO_OBJ_H__ */
