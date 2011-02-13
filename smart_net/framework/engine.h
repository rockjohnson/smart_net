/*
 * shared_obj.h
 *
 *  Created on: Feb 10, 2011
 *      Author: rock
 */

#ifndef __SHARED_OBJ_H__
#define __SHARED_OBJ_H__

#include "io_task.h"

namespace nm_framework
{
	/**
	 * the smart net manager.
	 * */
	class CNetEngine __SUPPORT_REF_CNT__
	{
		public:
			CNetEngine();
			virtual ~CNetEngine();

		public:
			int32_t add_io_obj();

		private:
			typedef std::set<io_task_ptr_t> io_task_set_t;

	};
	typedef nm_utils::CSmartPtr<nm_framework::CNetEngine> net_engine_ptr_t;

}

#endif /* __SHARED_OBJ_H__ */
