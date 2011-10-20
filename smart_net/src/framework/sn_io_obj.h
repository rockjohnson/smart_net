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

namespace nm_framework
{
	/**
	 * base class for io objs.
	 * */
	class IIoObj: public virtual nm_cmn_base::ICommonBase
	{
	public:
		IIoObj() :
			m_i32InputTaskId(0), m_i32OutputTaskId(0), m_ui32InputEvt(0), m_ui32OutputEvt(0)
		{
		}
		virtual ~IIoObj()
		{
		}

	public:
		virtual void handle_input_evt() = 0;
		virtual void handle_output_evt() = 0;
		virtual void handle_io_error(int32_t i32ErrCode) = 0;
		virtual void handle_add_into_io_task(int32_t i32IoType, int32_t i32ReturnCode) = 0;
		virtual void handle_del_from_io_task(int32_t i32IoType) = 0;
		virtual sock_handle_t get_ioobj_handle() = 0;

		void set_io_evt(int32_t i32IoType, u_int32_t ui32IoEvt)
		{
			switch (i32IoType)
			{
				case EIT_INPUT_TYPE:
				{
					m_ui32InputEvt = ui32IoEvt;
					break;
				}
				case EIT_OUTPUT_TYPE:
				{
					m_ui32OutputEvt = ui32IoEvt;
					break;
				}
				default:
				{
					CMN_ASSERT(false);
					break;
				}
			}
		}

		u_int32_t get_io_evt(int32_t i32IoType)
		{
			u_int32_t ui32IoEvt = 0;
			switch (i32IoType)
			{
				case EIT_INPUT_TYPE:
				{
					ui32IoEvt = m_ui32InputEvt;
				}
				case EIT_OUTPUT_TYPE:
				{
					ui32IoEvt = m_ui32OutputEvt;
				}
				default:
				{
					CMN_ASSERT(false);
					break;
				}
			}

			return ui32IoEvt;
		}

		void set_task_id(int32_t i32IoType, int32_t i32TaskId)
		{
			switch (i32IoType)
			{
				case EIT_INPUT_TYPE:
				{
					m_i32InputTaskId = i32TaskId;
					break;
				}
				case EIT_OUTPUT_TYPE:
				{
					m_i32OutputTaskId = i32TaskId;
					break;
				}
				default:
				{
					CMN_ASSERT(false);
					break;
				}
			}
		}

		int32_t get_task_id(int32_t i32IoType)
		{
			int32_t i32TaskId = 0;
			switch (i32IoType)
			{
				case EIT_INPUT_TYPE:
				{
					i32TaskId = m_i32InputTaskId;
					break;
				}
				case EIT_OUTPUT_TYPE:
				{
					i32TaskId = m_i32OutputTaskId;
					break;
				}
				default:
				{
					CMN_ASSERT(false);
					break;
				}
			}

			return i32TaskId;
		}

	private:
		int32_t m_i32InputTaskId;
		int32_t m_i32OutputTaskId;
		u_int32_t m_ui32InputEvt;
		u_int32_t m_ui32OutputEvt;
	};
	typedef nm_utils::CSmartPtr<IIoObj> io_obj_ptr_t;

}

#endif /* __IO_OBJ_H__ */
