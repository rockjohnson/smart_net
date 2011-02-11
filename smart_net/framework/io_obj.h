/*
 * io_obj.h
 *
 *  Created on: Dec 29, 2010
 *      Author: rock
 */

#ifndef _IO_OBJ_H___
#define _IO_OBJ_H___

#include <common/common.h>

namespace nm_framework
{

	class CIoObj __SUPPORT_REF_CNT__
	{
		public:
		CIoObj();
		virtual ~CIoObj();
	};

	typedef nm_utils::CSmartPtr<nm_framework::CIoObj> io_obj_ptr_t;

}

#endif /* __IO_OBJ_H__ */
